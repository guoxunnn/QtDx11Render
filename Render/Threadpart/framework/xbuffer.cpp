#include "xbuffer.h"

#include <string.h>
namespace framework {
// exchange-add operation for atomic operations on reference counters
#if defined __INTEL_COMPILER && !(defined WIN32 || defined _WIN32)
// atomic increment on the linux version of the Intel(tm) compiler
#define XADD_DEF(addr, delta) \
    (int)_InterlockedExchangeAdd(const_cast<void *>(reinterpret_cast<volatile void *>(addr)), delta)
#elif defined __GNUC__
#if defined __clang__ && __clang_major__ >= 3 && !defined __ANDROID__ && !defined __EMSCRIPTEN__ && \
    !defined(__CUDACC__)
#ifdef __ATOMIC_ACQ_REL
#define XADD_DEF(addr, delta) __c11_atomic_fetch_add((_Atomic(int) *)(addr), delta, __ATOMIC_ACQ_REL)
#else
#define XADD_DEF(addr, delta) __atomic_fetch_add((_Atomic(int) *)(addr), delta, 4)
#endif
#else
#if defined __ATOMIC_ACQ_REL && !defined __clang__
// version for gcc >= 4.7
#define XADD_DEF(addr, delta) (int)__atomic_fetch_add((unsigned *)(addr), (unsigned)(delta), __ATOMIC_ACQ_REL)
#else
#define XADD_DEF(addr, delta) (int)__sync_fetch_and_add((unsigned *)(addr), (unsigned)(delta))
#endif
#endif
#elif defined _MSC_VER && !defined RC_INVOKED
#include <intrin.h>
#define XADD_DEF(addr, delta) (int)_InterlockedExchangeAdd((long volatile *)addr, delta)
#else
static inline int XADD_DEF(int *addr, int delta) {
    int tmp = *addr;
    *addr += delta;
    return tmp;
}
#endif
enum DeviceType { DEVICE_CPU = 0, DEVICE_OTHER };

template <DeviceType DevType>
struct Allocator {
    inline void *New(size_t nbytes) const { return malloc(nbytes); }

    inline void Delete(void *data) const {
        if (data) {
            free(data);
            data = nullptr;
        }
    }
};
using MemAllocator = Allocator<DeviceType::DEVICE_CPU>;
XBuffer::XBuffer() : data_(nullptr), data_len_(0), refcount_(nullptr), release_cb_(nullptr) {}

XBuffer::XBuffer(size_t size) : data_(nullptr), data_len_(size), refcount_(nullptr), release_cb_(nullptr), user_data_(nullptr) {
    Create(size);
}

XBuffer::XBuffer(const char *data, size_t size)
    : data_(const_cast<char *>(data)), data_len_(size), refcount_(nullptr), release_cb_(nullptr), user_data_(nullptr) {}

XBuffer::XBuffer(const XBuffer &data)
    : data_(data.data_), data_len_(data.data_len_), refcount_(data.refcount_), release_cb_(data.release_cb_), user_data_(data.user_data_) {
    if (refcount_) { XADD_DEF(refcount_, 1); }
}

XBuffer &XBuffer::operator=(const XBuffer &data) {
    if (this != &data) {
        if (data.refcount_) XADD_DEF(data.refcount_, 1);

        Release();

        this->data_ = data.data_;
        this->data_len_ = data.data_len_;
        this->refcount_ = data.refcount_;
        this->release_cb_ = data.release_cb_;
        this->user_data_ = data.user_data_;
    }
    return *this;
}

XBuffer::~XBuffer() { Release(); }

void XBuffer::Release() {
    if (refcount_ && XADD_DEF(refcount_, -1) == 1) {
        if (release_cb_) release_cb_(data_, data_len_, user_data_);
        MemAllocator().Delete(data_);
    }

    data_ = nullptr;
    data_len_ = 0;
    refcount_ = nullptr;
}

bool XBuffer::Create(size_t size) {
    constexpr size_t refcount_size = sizeof(int);
    size_t total_align_size = 0;
    if (!refcount_ || (refcount_ && *refcount_ > 1) ||
        (refcount_ && *refcount_ <= 1 && (size > (size_t)((char *)refcount_ - data_)))) {
        Release();

        // align refcount;
        total_align_size = (size + 3) >> 2 << 2;
        data_ = (char *)MemAllocator().New(total_align_size + refcount_size);
        if (data_) {
            refcount_ = (int *)(data_ + total_align_size);
            *refcount_ = 1;
        }
    }

    data_len_ = size;

    return true;
}

XBuffer XBuffer::Clone() const {
    if (Empty()) return XBuffer();

    XBuffer dst;
    CopyTo(dst);

    return dst;
}

void XBuffer::CopyTo(XBuffer &dst) const {
    dst.Create(data_len_);
    dst.Write(data_, data_len_);
}

bool XBuffer::Write(char *data, size_t size, size_t pos) {
    size_t data_len = data_len_;
    if (data_len >= (pos + size) && data && size > 0) {
        memcpy(&data_[pos], data, size);
        return true;
    }
    return false;
}

}  // namespace framework

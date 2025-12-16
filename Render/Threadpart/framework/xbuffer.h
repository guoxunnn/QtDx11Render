#ifndef xbuffer_h
#define xbuffer_h
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

namespace framework {
class XBuffer {
    typedef void (*ReleaseCallback)(char *, size_t, void*);

public:
    XBuffer();
    explicit XBuffer(size_t size);
    XBuffer(const char *data, size_t size);
    XBuffer(const XBuffer &data);
    XBuffer &operator=(const XBuffer &data);
    virtual ~XBuffer();

    inline char *Data() { return data_; }
    inline const char *Data() const { return data_; }

    XBuffer Clone() const;
    void CopyTo(XBuffer &dst) const;

    virtual size_t DataSize() const { return data_len_; }
    virtual bool Empty() const { return data_ == nullptr || DataSize() == 0; }
    virtual bool Create(size_t size);
    virtual void Release();
    virtual bool Write(char *data, size_t size, size_t pos = 0);
    inline void SetReleaseCallback(ReleaseCallback cb, void* user_data) {
        release_cb_ = cb; user_data_
            = user_data;
    }

protected:
    char *data_;
    size_t data_len_;
    int *refcount_;
    ReleaseCallback release_cb_;
    void* user_data_;
};
using XString = XBuffer;
}  // namespace framework
#endif /* buffer_h */

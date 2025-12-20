//
//  singleton.hpp
//  thread_pool_test
//
//  Created by 田先润 on 2023/5/14.
//

#ifndef singleton_hpp
#define singleton_hpp

#include <stdio.h>
#include <stdlib.h>

#include <functional>
#include <mutex>

namespace base {
#if defined(__SUPPORT_TS_ANNOTATION__) || defined(__clang__)
#define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)  // no-op
#endif

#if !defined(__ANDROID__) && !defined(__linux__) && !defined(_WIN32)
#define GUARDED_BY(x) __attribute__((guarded_by(x)))
#define EXCLUSIVE_LOCKS_REQUIRED(...) __attribute__((exclusive_locks_required(__VA_ARGS__)))
#else
#define GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))
#define EXCLUSIVE_LOCKS_REQUIRED(...) THREAD_ANNOTATION_ATTRIBUTE__(exclusive_locks_required(__VA_ARGS__))
#endif

template <typename T>
class Singleton {
public:
    static T& Instance() EXCLUSIVE_LOCKS_REQUIRED(lock_) {
        if (!instance_) {
            std::lock_guard<std::mutex> guard(lock_);
            if (!instance_) {
                instance_ = new T;
                atexit(Singleton<T>::Release);
            }
        }
        return *instance_;
    }

private:
    Singleton() = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    static void Release() { delete Singleton<T>::instance_; }

    static T* instance_ GUARDED_BY(lock_);
    static std::mutex lock_;
};

template <typename T>
T* Singleton<T>::instance_ = nullptr;

template <typename T>
std::mutex Singleton<T>::lock_;
}  // namespace base
#endif /* singleton_hpp */

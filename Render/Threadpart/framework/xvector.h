#ifndef xvector_h
#define xvector_h

#include "stddef.h"
namespace framework {
template <class T>
class XVector {
public:
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;

public:
    XVector();
    explicit XVector(size_type n);
    XVector(size_type n, const value_type& value);
    XVector(const XVector& x);
    XVector(XVector&& x);
    ~XVector();

    XVector& operator=(const XVector& x);
    XVector& operator=(XVector&& x);
    void assign(size_type n, const value_type& u);

    size_type size() const;
    size_type max_size() const;
    size_type capacity() const;
    bool empty() const;
    void reserve(size_type n);
    void shrink_to_fit();

    reference operator[](size_type n);
    const_reference operator[](size_type n) const;
    reference at(size_type n);
    const_reference at(size_type n) const;

    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    void pop_back();
    void push_back(const_reference __x);
    void push_back(value_type&& __x);

    value_type* data();
    const value_type* data() const;

    void clear();

    void resize(size_type sz);
    void resize(size_type sz, const value_type& c);

    void swap(XVector& x);

private:
    void* val;
};
}  // namespace framework
#endif

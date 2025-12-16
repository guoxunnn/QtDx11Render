#include "xvector.h"

#include <stdint.h>

#include <vector>

#include "xbuffer.h"
#include "ximage.h"

using namespace std;
namespace framework {
template <typename T>
inline std::vector<T>* cv_(void* v) {
    return (std::vector<T>*)v;
}

template <typename T>
XVector<T>::XVector() {
    val = new std::vector<T>;
}

template <typename T>
XVector<T>::XVector(size_type n) {
    val = new std::vector<T>(n);
}

template <typename T>
XVector<T>::XVector(size_type n, const value_type& value) {
    val = new std::vector<T>(n, value);
}

template <typename T>
XVector<T>::XVector(const XVector& x) {
    val = new std::vector<T>(*cv_<T>(x.val));
}

template <typename T>
XVector<T>::XVector(XVector&& x) {
    val = new std::vector<T>(std::move(*cv_<T>(x.val)));
}

template <typename T>
XVector<T>::~XVector() {
    delete cv_<T>(val);
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
XVector<T>& XVector<T>::operator=(const XVector& x) {
    *cv_<T>(val) = *cv_<T>(x.val);
    return *this;
}

template <typename T>
XVector<T>& XVector<T>::operator=(XVector&& x) {
    *cv_<T>(val) = std::move(*cv_<T>(x.val));
    return *this;
}

template <typename T>
void XVector<T>::assign(size_type n, const value_type& u) {
    cv_<T>(val)->assign(n, u);
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
typename XVector<T>::size_type XVector<T>::size() const {
    return cv_<T>(val)->size();
}

template <typename T>
typename XVector<T>::size_type XVector<T>::max_size() const {
    return cv_<T>(val)->max_size();
}

template <typename T>
typename XVector<T>::size_type XVector<T>::capacity() const {
    return cv_<T>(val)->capacity();
}

template <typename T>
bool XVector<T>::empty() const {
    return cv_<T>(val)->empty();
}

template <typename T>
void XVector<T>::reserve(size_type n) {
    return cv_<T>(val)->reserve(n);
}

template <typename T>
void XVector<T>::shrink_to_fit() {
    return cv_<T>(val)->shrink_to_fit();
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
typename XVector<T>::reference XVector<T>::operator[](size_type n) {
    return (*cv_<T>(val))[n];
}

template <typename T>
typename XVector<T>::const_reference XVector<T>::operator[](size_type n) const {
    return (*cv_<T>(val))[n];
}

template <typename T>
typename XVector<T>::reference XVector<T>::at(size_type n) {
    return cv_<T>(val)->at(n);
}

template <typename T>
typename XVector<T>::const_reference XVector<T>::at(size_type n) const {
    return cv_<T>(val)->at(n);
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
typename XVector<T>::reference XVector<T>::front() {
    return cv_<T>(val)->front();
}

template <typename T>
typename XVector<T>::const_reference XVector<T>::front() const {
    return cv_<T>(val)->front();
}

template <typename T>
typename XVector<T>::reference XVector<T>::back() {
    return cv_<T>(val)->back();
}

template <typename T>
typename XVector<T>::const_reference XVector<T>::back() const {
    return cv_<T>(val)->back();
}

template <typename T>
void XVector<T>::pop_back() {
    cv_<T>(val)->pop_back();
}

template <typename T>
void XVector<T>::push_back(const_reference __x) {
    cv_<T>(val)->push_back(__x);
}

template <typename T>
void XVector<T>::push_back(value_type&& __x) {
    cv_<T>(val)->push_back(std::forward<T>(__x));
}

template <typename T>
typename XVector<T>::value_type* XVector<T>::data() {
    return cv_<T>(val)->data();
}

template <typename T>
const typename XVector<T>::value_type* XVector<T>::data() const {
    return cv_<T>(val)->data();
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
void XVector<T>::clear() {
    cv_<T>(val)->clear();
}

template <typename T>
void XVector<T>::resize(size_type sz) {
    cv_<T>(val)->resize(sz);
}

template <typename T>
void XVector<T>::resize(size_type sz, const value_type& c) {
    cv_<T>(val)->resize(sz, c);
}

template <typename T>
void XVector<T>::swap(XVector& x) {
    cv_<T>(val)->swap(*cv_<T>(x.val));
}

}  // namespace framework
template class framework::XVector<framework::XBuffer>;
template class framework::XVector<framework::XImage>;
template class framework::XVector<int>;
template class framework::XVector<framework::XVector<int>>;
template class framework::XVector<float>;
template class framework::XVector<framework::XVector<float>>;
template class framework::XVector<char*>;
template class framework::XVector<framework::XVector<char*>>;
template class framework::XVector<size_t>;
template class framework::XVector<framework::XVector<size_t>>;
template class framework::XVector<char>;
template class framework::XVector<framework::XVector<char>>;
template class framework::XVector<int64_t>;
template class framework::XVector<framework::XVector<int64_t>>;
template class framework::XVector<uint32_t>;
template class framework::XVector<framework::XVector<uint32_t>>;
#if defined(__linux__) || defined(__unix__) || defined(linux) || defined(__linux)
// template class framework::XVector<uint64_t>;
//  template class framework::XVector < framework::XVector < uint64_t>>;
#else
template class framework::XVector<uint64_t>;
template class framework::XVector<framework::XVector<uint64_t>>;
#endif

#include "ximage.h"

#include <string.h>

#include <string>
namespace framework {
XImage::XImage()
    : framework::XBuffer(),
      width_(0),
      height_(0),
      y_(nullptr),
      u_(nullptr),
      v_(nullptr),
      ystride_(0),
      ustride_(0),
      vstride_(0),
      format_(GRAY) {}

XImage::XImage(size_t _width, size_t _height, PIXEL_FORMAT _format)
    : framework::XBuffer(), width_(_width), height_(_height), format_(_format) {
    Create(_width, _height, _format);
}

XImage::XImage(const XImage& rhs)
    : framework::XBuffer(rhs),
      width_(rhs.width_),
      height_(rhs.height_),
      y_(rhs.y_),
      u_(rhs.u_),
      v_(rhs.v_),
      ystride_(rhs.ystride_),
      ustride_(rhs.ustride_),
      vstride_(rhs.vstride_),
      format_(rhs.format_) {}

XImage& XImage::operator=(const XImage& rhs) {
    if (this == &rhs) return *this;

    framework::XBuffer::operator=(rhs);

    this->y_ = rhs.y_;
    this->u_ = rhs.u_;
    this->v_ = rhs.v_;
    this->width_ = rhs.width_;
    this->height_ = rhs.height_;
    this->ystride_ = rhs.ystride_;
    this->ustride_ = rhs.ustride_;
    this->vstride_ = rhs.vstride_;
    this->format_ = rhs.format_;
    return *this;
}

XImage::~XImage() { Release(); }

void XImage::Release() {
    framework::XBuffer::Release();
    y_ = u_ = v_ = nullptr;
    ystride_ = ustride_ = vstride_ = 0;
    width_ = height_ = 0;
}

bool XImage::Empty() const { return (y_ == nullptr || Total() == 0); }

size_t XImage::Total() const { return height_ * ystride_ + (ustride_ + vstride_) * height_ / 2; }

void XImage::Create(size_t _width, size_t _height, PIXEL_FORMAT _format, int _ystride, int _ustride,
                    int _vstride) {
    switch (_format) {
        case PIXEL_FORMAT::I420:
            if (_ystride <= 0) _ystride = _width;
            if (_vstride <= 0) _vstride = _width / 2;
            if (_ustride <= 0) _ustride = _width / 2;
            break;
        case PIXEL_FORMAT::NV12:
        case PIXEL_FORMAT::NV21:
            if (_ystride <= 0) _ystride = _width;
            if (_ustride <= 0) _ustride = _width;
            _vstride = 0;
            break;
        case PIXEL_FORMAT::GRAY:
            if (_ystride <= 0) _ystride = _width;
            _ustride = _vstride = 0;
            break;
        case PIXEL_FORMAT::RGBA:
        case PIXEL_FORMAT::BGRA:
            if (_ystride <= 0) _ystride = _width * 4;
            _ustride = _vstride = 0;
            break;
        case PIXEL_FORMAT::RGB:
        case PIXEL_FORMAT::BGR:
            if (_ystride <= 0) _ystride = _width * 3;
            _ustride = _vstride = 0;
            break;
        default: break;
    }
    uint32_t total_needed = _height * _ystride + (_ustride + _vstride) * _height / 2;

    framework::XBuffer::Create(total_needed);

    this->width_ = _width;
    this->height_ = _height;
    this->format_ = _format;
    this->ystride_ = _ystride;
    this->ustride_ = _ustride;
    this->vstride_ = _vstride;

    this->y_ = (unsigned char*)data_;
    this->u_ = nullptr;
    this->v_ = nullptr;
    if (ustride_) u_ = y_ + height_ * ystride_;
    if (vstride_) v_ = u_ + height_ * ustride_ / 2;
}

XImage XImage::CloneImage() const {
    if (this->Empty()) return XImage();

    XImage res;
    CopyTo(res);

    return res;
}

void XImage::CopyTo(XImage& dst) const {
    dst.Create(this->width_, this->height_, this->format_);
    if (dst.y_) memcpy(dst.y_, this->y_, this->height_ * this->ystride_);
    if (dst.u_) memcpy(dst.u_, this->u_, this->height_ * this->ustride_ / 2);
    if (dst.v_) memcpy(dst.v_, this->v_, this->height_ * this->vstride_ / 2);
}

XImage XImage::FromGray(size_t _width, size_t _height, char* _data, int _stride) {
    XImage res;
    res.data_ = _data;
    res.width_ = _width;
    res.height_ = _height;
    res.format_ = PIXEL_FORMAT::GRAY;

    res.stride_ = _stride <= 0 ? _width : _stride;
    res.data_len_ = (uint32_t)res.Total();
    res.y_ = (unsigned char*)_data;
    res.u_ = nullptr;
    res.v_ = nullptr;
    res.ystride_ = _stride <= 0 ? _width * 1 : _stride;
    res.ustride_ = 0;
    res.vstride_ = 0;
    return res;
}

XImage XImage::FromBgra(int _width, int _height, uint8_t* _data, int _stride /*= 0*/) {
    XImage dst;
    dst.format_ = BGRA;
    dst.y_ = _data;
    dst.u_ = nullptr;
    dst.v_ = nullptr;
    dst.ystride_ = _stride <= 0 ? _width * 4 : _stride;
    dst.ustride_ = 0;
    dst.vstride_ = 0;
    dst.width_ = _width;
    dst.height_ = _height;

    return dst;
}

XImage XImage::FromRgba(int _width, int _height, uint8_t* _data, int _stride /*= 0*/) {
    XImage out;
    out.format_ = RGBA;
    out.data_ = (char*)_data;
    out.y_ = _data;
    out.u_ = nullptr;
    out.v_ = nullptr;
    out.ystride_ = _stride <= 0 ? _width * 4 : _stride;
    out.ustride_ = 0;
    out.vstride_ = 0;
    out.width_ = _width;
    out.height_ = _height;

    out.data_len_ = (uint32_t)out.Total();
    return out;
}

XImage XImage::FromRgb(int _width, int _height, uint8_t* _data, int _stride) {
    XImage out;
    out.format_ = RGB;
    out.data_ = (char*)_data;
    out.y_ = _data;
    out.u_ = nullptr;
    out.v_ = nullptr;
    out.ystride_ = _stride <= 0 ? _width * 3 : _stride;
    out.ustride_ = 0;
    out.vstride_ = 0;
    out.width_ = _width;
    out.height_ = _height;
    out.data_len_ = (uint32_t)out.Total();
    return out;
}

size_t XImage::Channel() const {
        size_t depth = 1;
        switch (format_) {
            case PIXEL_FORMAT::GRAY: {
                depth = 1;
            } break;
            case PIXEL_FORMAT::RGB:
            case PIXEL_FORMAT::BGR: {
                depth = 3;
            } break;
            case PIXEL_FORMAT::RGBA:
            case PIXEL_FORMAT::BGRA: {
                depth = 4;
            } break;
            default: {
                depth = 0;
            } break;
        }

        return depth;
    }
}  // namespace framework

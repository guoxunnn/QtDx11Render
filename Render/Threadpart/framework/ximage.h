#ifndef ximage_h
#define ximage_h
#include <stdint.h>

#include "xbuffer.h"
namespace framework {
class XImage : public framework::XBuffer {
public:
    enum PIXEL_FORMAT { GRAY, RGBA, NV12, NV21, I420, RGB, BGRA, BGR };

    XImage();
    XImage(size_t _width, size_t _height, PIXEL_FORMAT _format = PIXEL_FORMAT::GRAY);
    XImage(const XImage& rhs);
    XImage& operator=(const XImage& rhs);
    virtual ~XImage();

    virtual void Release() override;
    virtual bool Empty() const override;
    virtual size_t DataSize() const { return Total(); }
    size_t Total() const;
    void Create(size_t _width, size_t _height, PIXEL_FORMAT _format = PIXEL_FORMAT::GRAY, int _ystride = 0,
                int _ustride = 0, int _vstride = 0);
    XImage CloneImage() const;
    void CopyTo(XImage& dst) const;

    static XImage FromGray(size_t _width, size_t _height, char* _data, int _stride = 0);
    static XImage FromRgba(int _width, int _height, uint8_t* _data, int _stride = 0);
    static XImage FromRgb(int _width, int _height, uint8_t* _data, int _stride = 0);
    static XImage FromBgra(int _width, int _height, uint8_t* _data, int _stride = 0);

    inline size_t Width() const { return width_; }
    inline size_t Height() const { return height_; }
    size_t Channel() const;
    inline PIXEL_FORMAT PixelFormat() const { return format_; }

private:
    unsigned char* y_;
    unsigned char* u_;
    unsigned char* v_;
    union {
        int ystride_;
        int stride_;
    };
    int ustride_;
    int vstride_;

    int width_;
    int height_;
    PIXEL_FORMAT format_;
};
}  // namespace framework
#endif /* ximage_h */

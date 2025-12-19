#include "RenderData.h"

namespace render {

RenderColor::RenderColor()
    : red_(0.f), green_(0.f), blue_(0.f), alpha_(1.f) {}

RenderColor::RenderColor(float r, float g, float b, float a)
    : red_(r), green_(g), blue_(b), alpha_(a) {}

RenderColor RenderColor::FromColor32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return RenderColor(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

RenderColor RenderColor::FromColor32(uint32_t rgba) {
    return FromColor32((rgba & 0x000000FF), (rgba & 0x0000FF00) >> 8, (rgba & 0x00FF0000) >> 16, (rgba & 0xFF000000) >> 24);
}

bool RenderColor::operator==(const RenderColor& other) const {
    constexpr float tol = 0.499999 / 256;
    return std::abs(red_ - other.red_) <= tol && std::abs(green_ - other.green_) <= tol &&
           std::abs(blue_ - other.blue_) <= tol && std::abs(alpha_ - other.alpha_) <= tol;
}

bool RenderColor::operator!=(const RenderColor& other) const {
    return !operator==(other);
}
}

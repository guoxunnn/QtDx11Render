/*
 * Copyright (C) 2017,2018,2019,2020 深圳市创必得科技有限公司
 */
#include "FVector2D.h"

#include <cmath>

#include "FVector3D.h"
namespace geometry {
FVector2D::FVector2D() : mX(0.0), mY(0.0) {}

FVector2D::FVector2D(float xpos, float ypos) : mX(xpos), mY(ypos) {}

FVector2D::FVector2D(const FVector3D& fVector_) {
    mX = fVector_.x();
    mY = fVector_.y();
}

FVector2D FVector2D::normalized() const {
    float len = mX * mX + mY * mY;
    if (MathUtil::FuzzyIsNull(len - 1.0)) {
        return *this;
    } else if (!MathUtil::FuzzyIsNull(len)) {
        float sqrtLen = std::sqrt(len);
        return FVector2D(mX / sqrtLen, mY / sqrtLen);
    } else {
        return FVector2D();
    }
}

void FVector2D::normalize() {
    float len = mX * mX + mY * mY;
    if (MathUtil::FuzzyIsNull(len - 1.0) || MathUtil::FuzzyIsNull(len)) return;

    len = std::sqrt(len);

    mX /= len;
    mY /= len;
}

float FVector2D::distanceToLine(const FVector2D& point, const FVector2D& direction) const {
    if (direction.isNull()) return (*this - point).length();
    FVector2D p = point + dotProduct(*this - point, direction) * direction;
    return (*this - p).length();
}
}  // namespace geometry
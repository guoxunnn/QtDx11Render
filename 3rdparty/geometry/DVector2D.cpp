/*
 * Copyright (C) 2017,2018,2019,2020 深圳市创必得科技有限公司
 */
#include "DVector2D.h"
#include "DVector3D.h"

#include <cmath>
namespace geometry {
DVector2D::DVector2D() : mX(0.0), mY(0.0) {}

DVector2D::DVector2D(double xpos, double ypos) : mX(xpos), mY(ypos) {}

DVector2D::DVector2D(const DVector3D & vector) {
    mX = vector.x();
    mY = vector.y();
}

DVector2D DVector2D::normalized() const {
    double len = mX * mX + mY * mY;
    if (MathUtil::FuzzyIsNull(len - 1.0)) {
        return *this;
    } else if (!MathUtil::FuzzyIsNull(len)) {
        double sqrtLen = std::sqrt(len);
        return DVector2D(mX / sqrtLen, mY / sqrtLen);
    } else {
        return DVector2D();
    }
}

void DVector2D::normalize() {
    double len = mX * mX + mY * mY;
    if (MathUtil::FuzzyIsNull(len - 1.0) || MathUtil::FuzzyIsNull(len)) return;

    len = std::sqrt(len);

    mX /= len;
    mY /= len;
}

double DVector2D::distanceToLine(const DVector2D& point, const DVector2D& direction) const {
    if (direction.isNull()) return (*this - point).length();
    DVector2D p = point + dotProduct(*this - point, direction) * direction;
    return (*this - p).length();
}
}  // namespace geometry
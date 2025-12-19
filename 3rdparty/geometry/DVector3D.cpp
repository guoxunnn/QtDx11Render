/*
 * Copyright (C) 2017,2018,2019,2020 深圳市创必得科技有限公司
 */
#include "DVector3D.h"

#include "FVector3D.h"
#include <cmath>
namespace geometry {
DVector3D::DVector3D(const FVector3D& vector) {
    mX = vector.x();
    mY = vector.y();
    mZ = vector.z();
}

DVector3D& DVector3D::operator=(const FVector3D& vector) {
    mX = vector.x();
    mY = vector.y();
    mZ = vector.z();
    return *this;
}

DVector3D::DVector3D() : mX(0), mY(0), mZ(0) {}

DVector3D::DVector3D(double xpos, double ypos, double zpos) {
    mX = xpos;
    mY = ypos;
    mZ = zpos;
}

DVector3D::DVector3D(const DVector2D& vector, double zpos) {
    mX = vector.x();
    mY = vector.y();
    mZ = zpos;
}

DVector3D DVector3D::normalized() const {
    double len = mX * mX + mY * mY + mZ * mZ;
    if (MathUtil::FuzzyIsNull(len - 1.0)) {
        return *this;
    } else if (!MathUtil::FuzzyIsNull(len)) {
        double sqrtLen = std::sqrt(len);
        return DVector3D(mX / sqrtLen, mY / sqrtLen, mZ / sqrtLen);
    } else {
        return DVector3D();
    }
}

void DVector3D::normalize() {
    double len = mX * mX + mY * mY + mZ * mZ;
    if (MathUtil::FuzzyIsNull(len - 1.0) || MathUtil::FuzzyIsNull(len)) return;

    len = std::sqrt(len);

    mX /= len;
    mY /= len;
    mZ /= len;
}

double DVector3D::distanceToLine(const DVector3D& point, const DVector3D& direction) const {
    if (direction.isNull()) return (*this - point).length();
    DVector3D p = point + dotProduct(*this - point, direction) * direction;
    return (*this - p).length();
}

void DVector3D::toMat3x3(double m_[3][3]) {
    m_[0][0] = mX;
    m_[0][1] = 0;
    m_[0][2] = 0;
    m_[1][0] = 0;
    m_[1][1] = mY;
    m_[1][2] = 0;
    m_[2][0] = 0;
    m_[2][1] = 0;
    m_[2][2] = mZ;
}

void DVector3D::toMat4x4(double m_[4][4]) {
    m_[0][0] = mX;
    m_[0][1] = 0;
    m_[0][2] = 0;
    m_[0][3] = 0;
    m_[1][0] = 0;
    m_[1][1] = mY;
    m_[1][2] = 0;
    m_[1][3] = 0;
    m_[2][0] = 0;
    m_[2][1] = 0;
    m_[2][2] = mZ;
    m_[2][3] = 0;
    m_[3][0] = 0;
    m_[3][1] = 0;
    m_[3][2] = 0;
    m_[3][3] = 1.0;
}

FVector3D DVector3D::toFVector3D() const { return FVector3D((float)mX, (float)mY, (float)mZ); }

float DVector3D::getAngleTo(const DVector3D & v) const {
    if(MathUtil::FuzzyIsNull(length()) || MathUtil::FuzzyIsNull(v.length())) {
        return 0;
    }
    DVector3D v1 = normalized();
    DVector3D v2 = v.normalized();
    float dot = DVector3D::dotProduct(v1, v2);
    float rad = std::acos(dot);
    return MathUtil::RadianToDegree(rad);
}

float DVector3D::getAngleTo(const DVector3D & v, const DVector3D & normal) const {
    float angle = getAngleTo(v);
    DVector3D n = DVector3D::crossProduct(*this, v);
    if(DVector3D::dotProduct(n, normal) < 0) {
        angle = 360 - angle;
    }
    return angle;
}
}  // namespace geometry

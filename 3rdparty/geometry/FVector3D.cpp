/*
 * Copyright (C) 2017,2018,2019,2020 深圳市创必得科技有限公司
 */
#include "FVector3D.h"

#include "DVector3D.h"
#include "FVector4D.h"

namespace geometry {
FVector3D::FVector3D() {
    this->mX = 0;
    this->mY = 0;
    this->mZ = 0;
}

FVector3D::FVector3D(float x_, float y_, float z_) {
    mX = x_;
    mY = y_;
    mZ = z_;
}

FVector3D::FVector3D(const FVector2D& vector, float zpos) {
    mX = vector.x();
    mY = vector.y();
    mZ = zpos;
}

FVector3D::FVector3D(const FVector3D& vector) {
    mX = vector.x();
    mY = vector.y();
    mZ = vector.z();
}

FVector3D::FVector3D(const FVector4D& vector) {
    mX = vector.x();
    mY = vector.y();
    mZ = vector.z();
}

FVector3D::FVector3D(const DVector3D& vector) {
    mX = vector.x();
    mY = vector.y();
    mZ = vector.z();
}
FVector3D& FVector3D::operator=(const DVector3D& vector) {
    mX = vector.x();
    mY = vector.y();
    mZ = vector.z();
    return *this;
}
FVector3D FVector3D::normalized() const {
    double len = lengthD();
    if (MathUtil::FuzzyIsNull(len - 1.0)) {
        return *this;
    } else if (!MathUtil::FuzzyIsNull(len)) {
        double sqrtLen = std::sqrt(len);
        return FVector3D(float(double(mX) / sqrtLen), float(double(mY) / sqrtLen),
                         float(double(mZ) / sqrtLen));
    } else {
        return FVector3D();
    }
}

void FVector3D::normalize() {
    double len = lengthD();
    if (MathUtil::FuzzyIsNull(len - 1.0) || MathUtil::FuzzyIsNull(len)) return;

    len = std::sqrt(len);

    mX = float(double(mX) / len);
    mY = float(double(mY) / len);
    mZ = float(double(mZ) / len);
}

float FVector3D::distanceToPoint(const FVector3D& point) const { return (*this - point).length(); }

float FVector3D::distanceToLine(const FVector3D& point, const FVector3D& direction) const {
    if (direction.isNull()) return (*this - point).length();
    FVector3D p = point + dotProduct(*this - point, direction) * direction;
    return (*this - p).length();
}

void FVector3D::toMat3x3(float m_[3][3]) {
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

void FVector3D::toMat4x4(float m_[4][4]) {
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

float FVector3D::dotProduct(const FVector3D& v1, const FVector3D& v2) {
    return v1.mX * v2.mX + v1.mY * v2.mY + v1.mZ * v2.mZ;
}

FVector3D FVector3D::crossProduct(const FVector3D& v1, const FVector3D& v2) {
    return FVector3D(v1.mY * v2.mZ - v1.mZ * v2.mY, v1.mZ * v2.mX - v1.mX * v2.mZ,
                     v1.mX * v2.mY - v1.mY * v2.mX);
}

FVector3D FVector3D::normal(const FVector3D& v1, const FVector3D& v2) {
    return crossProduct(v1, v2).normalized();
}

FVector3D FVector3D::normal(const FVector3D& v1, const FVector3D& v2, const FVector3D& v3) {
    return crossProduct((v2 - v1), (v3 - v1)).normalized();
}

float FVector3D::getAngleTo(const FVector3D & v) const {
    if(MathUtil::FuzzyIsNull(length()) || MathUtil::FuzzyIsNull(v.length())) {
        return 0;
    }
    FVector3D v1 = normalized();
    FVector3D v2 = v.normalized();
    float dot = FVector3D::dotProduct(v1, v2);
    float rad = std::acos(dot);
    return MathUtil::RadianToDegree(rad);
}

float FVector3D::getAngleTo(const FVector3D & v, const FVector3D & normal) const {
    float angle = getAngleTo(v);
    FVector3D n = FVector3D::crossProduct(*this, v);
    if(FVector3D::dotProduct(n, normal) < 0) {
        angle = 360 - angle;
    }
    return angle;
}
}  // namespace geometry

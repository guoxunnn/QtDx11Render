///*
// * Copyright (C) 2017,2018,2019,2020 深圳市创必得科技有限公司
// */
//#include "FQuaternion.h"

//#include <assert.h>

//#include <cmath>
//#include <cstddef>
//#include <cstdint>
//#include <iostream>
//#include <memory>

//namespace geometry {
//// 当前QQuaternion中无此设置
//void FQuaternion::setToIdentity() {
//    wp = 1.0;
//    xp = 0.0;
//    yp = 0.0;
//    zp = 0.0;
//}

//FVector3D FQuaternion::rotatedVector(const FVector3D& vector) const {
//    return (*this * FQuaternion(0, vector) * conjugated()).vector();
//}

//void FQuaternion::toMat3x3(float m[3][3]) {
//    const float f2x = xp + xp;
//    const float f2y = yp + yp;
//    const float f2z = zp + zp;
//    const float f2xw = f2x * wp;
//    const float f2yw = f2y * wp;
//    const float f2zw = f2z * wp;
//    const float f2xx = f2x * xp;
//    const float f2xy = f2x * yp;
//    const float f2xz = f2x * zp;
//    const float f2yy = f2y * yp;
//    const float f2yz = f2y * zp;
//    const float f2zz = f2z * zp;

//    m[0][0] = 1.0 - (f2yy + f2zz);
//    m[0][1] = f2xy - f2zw;
//    m[0][2] = f2xz + f2yw;
//    m[1][0] = f2xy + f2zw;
//    m[1][1] = 1.0 - (f2xx + f2zz);
//    m[1][2] = f2yz - f2xw;
//    m[2][0] = f2xz - f2yw;
//    m[2][1] = f2yz + f2xw;
//    m[2][2] = 1.0 - (f2xx + f2yy);
//}

//void FQuaternion::toMat4x4(float m[4][4]) {
//    const float f2x = xp + xp;
//    const float f2y = yp + yp;
//    const float f2z = zp + zp;
//    const float f2xw = f2x * wp;
//    const float f2yw = f2y * wp;
//    const float f2zw = f2z * wp;
//    const float f2xx = f2x * xp;
//    const float f2xy = f2x * yp;
//    const float f2xz = f2x * zp;
//    const float f2yy = f2y * yp;
//    const float f2yz = f2y * zp;
//    const float f2zz = f2z * zp;

//    m[0][0] = 1.0 - (f2yy + f2zz);
//    m[0][1] = f2xy - f2zw;
//    m[0][2] = f2xz + f2yw;
//    m[0][3] = 0.0;
//    m[1][0] = f2xy + f2zw;
//    m[1][1] = 1.0 - (f2xx + f2zz);
//    m[1][2] = f2yz - f2xw;
//    m[1][3] = 0.0;
//    m[2][0] = f2xz - f2yw;
//    m[2][1] = f2yz + f2xw;
//    m[2][2] = 1.0 - (f2xx + f2yy);
//    m[2][3] = 0.0;
//    m[3][0] = 0.0;
//    m[3][1] = 0.0;
//    m[3][2] = 0.0;
//    m[3][3] = 1.0;  // 注意最后这个元素为1，参考 QMatrix4x4的构造函数QMatrix4x4(const QGenericMatrix<N, M,
//                    // float> &matrix = N)的说明
//}

//FQuaternion FQuaternion::normalized() const {
//    // Need some extra precision if the length is very small.
//    float len = float(xp) * float(xp) + float(yp) * float(yp) + float(zp) * float(zp) + float(wp) * float(wp);
//    if (MathUtil::FuzzyIsNull(len - 1.0))
//        return *this;
//    else if (!MathUtil::FuzzyIsNull(len))
//        return *this / std::sqrt(len);
//    else
//        return FQuaternion(0.0, 0.0, 0.0, 0.0);
//}

//void FQuaternion::getEulerAngles(float* pitch, float* yaw, float* roll) const {
//    // Algorithm from:
//    // http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q37

//    float xx = xp * xp;
//    float xy = xp * yp;
//    float xz = xp * zp;
//    float xw = xp * wp;
//    float yy = yp * yp;
//    float yz = yp * zp;
//    float yw = yp * wp;
//    float zz = zp * zp;
//    float zw = zp * wp;

//    const float lengthSquared = xx + yy + zz + wp * wp;
//    if (!MathUtil::FuzzyIsNull(lengthSquared - 1.0f) && !MathUtil::FuzzyIsNull(lengthSquared)) {
//        xx /= lengthSquared;
//        xy /= lengthSquared;  // same as (xp / length) * (yp / length)
//        xz /= lengthSquared;
//        xw /= lengthSquared;
//        yy /= lengthSquared;
//        yz /= lengthSquared;
//        yw /= lengthSquared;
//        zz /= lengthSquared;
//        zw /= lengthSquared;
//    }

//    *pitch = std::asin(-2.0f * (yz - xw));
//    if (*pitch < CBD_M_PI_2) {
//        if (*pitch > -CBD_M_PI_2) {
//            *yaw = std::atan2(2.0f * (xz + yw), 1.0f - 2.0f * (xx + yy));
//            *roll = std::atan2(2.0f * (xy + zw), 1.0f - 2.0f * (xx + zz));
//        } else {
//            // not a unique solution
//            *roll = 0.0f;
//            *yaw = -std::atan2(-2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
//        }
//    } else {
//        // not a unique solution
//        *roll = 0.0f;
//        *yaw = std::atan2(-2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
//    }

//    *pitch = MathUtil::RadianToDegree(*pitch);
//    *yaw = MathUtil::RadianToDegree(*yaw);
//    *roll = MathUtil::RadianToDegree(*roll);
//}

//FQuaternion FQuaternion::fromEulerAngles(float pitch, float yaw, float roll) {
//    // Algorithm from:
//    // http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q60

//    pitch = MathUtil::DegreeToRadian(pitch);
//    yaw = MathUtil::DegreeToRadian(yaw);
//    roll = MathUtil::DegreeToRadian(roll);

//    return fromRadians(pitch, yaw, roll);
//}

//FQuaternion FQuaternion::fromRadians(float pitch, float yaw, float roll) {
//    pitch *= 0.5;
//    yaw *= 0.5;
//    roll *= 0.5;

//    const float c1 = std::cos(yaw);
//    const float s1 = std::sin(yaw);
//    const float c2 = std::cos(roll);
//    const float s2 = std::sin(roll);
//    const float c3 = std::cos(pitch);
//    const float s3 = std::sin(pitch);
//    const float c1c2 = c1 * c2;
//    const float s1s2 = s1 * s2;

//    const float w = c1c2 * c3 + s1s2 * s3;
//    const float x = c1c2 * s3 + s1s2 * c3;
//    const float y = s1 * c2 * c3 - c1 * s2 * s3;
//    const float z = c1 * s2 * c3 - s1 * c2 * s3;

//    return FQuaternion(w, x, y, z);
//}

//FQuaternion FQuaternion::rotationTo(const FVector3D& from, const FVector3D& to) {
//    // Based on Stan Melax's article in Game Programming Gems

//    const FVector3D v0(from.normalized());
//    const FVector3D v1(to.normalized());

//    float d = FVector3D::dotProduct(v0, v1) + 1.0;

//    // if dest vector is close to the inverse of source vector, ANY axis of rotation is valid
//    if (MathUtil::FuzzyIsNull(d)) {
//        FVector3D axis = FVector3D::crossProduct(FVector3D(1.0, 0.0, 0.0), v0);
//        if (MathUtil::FuzzyIsNull(axis.lengthSquared()))
//            axis = FVector3D::crossProduct(FVector3D(0.0, 1.0, 0.0), v0);
//        axis.normalize();

//        // same as QQuaternion::fromAxisAndAngle(axis, 180.0f)
//        return FQuaternion(0.0, axis.x(), axis.y(), axis.z());
//    }

//    d = std::sqrt(2.0 * d);
//    const FVector3D axis(FVector3D::crossProduct(v0, v1) / d);

//    return FQuaternion(d * 0.5, axis).normalized();
//}

//void FQuaternion::getAxisAndAngle(FVector3D* axis, float* angle) const {
//    const float length = std::hypot(xp, yp, zp);
//    if (!MathUtil::FuzzyIsNull(length)) {
//        *axis = FVector3D(xp, yp, zp);
//        if (MathUtil::FuzzyIsEqual(length, 1.0f)) {
//        } else {
//            (*axis) /= length;
//        }
//        *angle = MathUtil::RadianToDegree(2.0f * std::acos(wp));
//    } else {
//        // angle is 0 (mod 2*pi), so any axis will fit
//        *axis = FVector3D(0.f, 0.f, 0.f);
//        *angle = 0.f;
//    }
//}

//FQuaternion FQuaternion::fromAxisAndAngle(const FVector3D& axis, float angle) {
//    // Algorithm from:
//    // http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q56
//    // We normalize the result just in case the values are close
//    // to zero, as suggested in the above FAQ.
//    float a = MathUtil::DegreeToRadian(angle / 2.0f);
//    float s = std::sin(a);
//    float c = std::cos(a);
//    FVector3D ax = axis.normalized();
//    return FQuaternion(c, ax.x() * s, ax.y() * s, ax.z() * s).normalized();
//}

//FMatrix3x3 FQuaternion::toRotationMatrix() const {
//    FMatrix3x3 rot3x3(Uninitialized);

//    const float f2x = xp + xp;
//    const float f2y = yp + yp;
//    const float f2z = zp + zp;
//    const float f2xw = f2x * wp;
//    const float f2yw = f2y * wp;
//    const float f2zw = f2z * wp;
//    const float f2xx = f2x * xp;
//    const float f2xy = f2x * yp;
//    const float f2xz = f2x * zp;
//    const float f2yy = f2y * yp;
//    const float f2yz = f2y * zp;
//    const float f2zz = f2z * zp;

//    rot3x3(0, 0) = 1.0f - (f2yy + f2zz);
//    rot3x3(0, 1) = f2xy - f2zw;
//    rot3x3(0, 2) = f2xz + f2yw;
//    rot3x3(1, 0) = f2xy + f2zw;
//    rot3x3(1, 1) = 1.0f - (f2xx + f2zz);
//    rot3x3(1, 2) = f2yz - f2xw;
//    rot3x3(2, 0) = f2xz - f2yw;
//    rot3x3(2, 1) = f2yz + f2xw;
//    rot3x3(2, 2) = 1.0f - (f2xx + f2yy);

//    return rot3x3;
//}

//FQuaternion FQuaternion::fromRotationMatrix(const FMatrix3x3& rot3x3) {
//    // Algorithm from:
//    // http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q55

//    float scalar;
//    float axis[3];

//    const float trace = rot3x3(0, 0) + rot3x3(1, 1) + rot3x3(2, 2);
//    if (trace > 0.00000001f) {
//        const float s = 2.0f * std::sqrt(trace + 1.0f);
//        scalar = 0.25f * s;
//        axis[0] = (rot3x3(2, 1) - rot3x3(1, 2)) / s;
//        axis[1] = (rot3x3(0, 2) - rot3x3(2, 0)) / s;
//        axis[2] = (rot3x3(1, 0) - rot3x3(0, 1)) / s;
//    } else {
//        static int s_next[3] = {1, 2, 0};
//        int i = 0;
//        if (rot3x3(1, 1) > rot3x3(0, 0)) i = 1;
//        if (rot3x3(2, 2) > rot3x3(i, i)) i = 2;
//        int j = s_next[i];
//        int k = s_next[j];

//        const float s = 2.0f * std::sqrt(rot3x3(i, i) - rot3x3(j, j) - rot3x3(k, k) + 1.0f);
//        axis[i] = 0.25f * s;
//        scalar = (rot3x3(k, j) - rot3x3(j, k)) / s;
//        axis[j] = (rot3x3(j, i) + rot3x3(i, j)) / s;
//        axis[k] = (rot3x3(k, i) + rot3x3(i, k)) / s;
//    }

//    return FQuaternion(scalar, axis[0], axis[1], axis[2]);
//}
//FQuaternion FQuaternion::MulQuat(const FQuaternion& q1, const FQuaternion& q2) {
//    float yy = (q1.wp - q1.yp) * (q2.wp + q2.zp);
//    float zz = (q1.wp + q1.yp) * (q2.wp - q2.zp);
//    float ww = (q1.zp + q1.xp) * (q2.xp + q2.yp);
//    float xx = ww + yy + zz;
//    float qq = 0.5 * (xx + (q1.zp - q1.xp) * (q2.xp - q2.yp));

//    float y = qq - yy + (q1.wp - q1.xp) * (q2.yp + q2.zp);
//    float z = qq - zz + (q1.zp + q1.yp) * (q2.wp - q2.xp);
//    float w = qq - ww + (q1.zp - q1.yp) * (q2.yp - q2.zp);
//    float x = qq - xx + (q1.xp + q1.wp) * (q2.xp + q2.wp);

//    return FQuaternion(w, x, y, z);
//}
//}  // namespace geometry

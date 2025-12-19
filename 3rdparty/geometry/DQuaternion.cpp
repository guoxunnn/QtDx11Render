/*
 * Copyright (C) 2017,2018,2019,2020 深圳市创必得科技有限公司
 */
#include "DQuaternion.h"

#include <cmath>
namespace geometry {
// 当前QQuaternion中无此设置
void DQuaternion::setToIdentity() {
    wp = 1.0;
    xp = 0.0;
    yp = 0.0;
    zp = 0.0;
}

// #include <qmath.h>
DVector3D DQuaternion::rotatedVector(const DVector3D& vector) const {
    return (*this * DQuaternion(0, vector) * conjugated()).vector();
}

void DQuaternion::toMat3x3(double m[3][3]) {
    const double f2x = xp + xp;
    const double f2y = yp + yp;
    const double f2z = zp + zp;
    const double f2xw = f2x * wp;
    const double f2yw = f2y * wp;
    const double f2zw = f2z * wp;
    const double f2xx = f2x * xp;
    const double f2xy = f2x * yp;
    const double f2xz = f2x * zp;
    const double f2yy = f2y * yp;
    const double f2yz = f2y * zp;
    const double f2zz = f2z * zp;

    m[0][0] = 1.0 - (f2yy + f2zz);
    m[0][1] = f2xy - f2zw;
    m[0][2] = f2xz + f2yw;
    m[1][0] = f2xy + f2zw;
    m[1][1] = 1.0 - (f2xx + f2zz);
    m[1][2] = f2yz - f2xw;
    m[2][0] = f2xz - f2yw;
    m[2][1] = f2yz + f2xw;
    m[2][2] = 1.0 - (f2xx + f2yy);
}

void DQuaternion::toMat4x4(double m[4][4]) {
    const double f2x = xp + xp;
    const double f2y = yp + yp;
    const double f2z = zp + zp;
    const double f2xw = f2x * wp;
    const double f2yw = f2y * wp;
    const double f2zw = f2z * wp;
    const double f2xx = f2x * xp;
    const double f2xy = f2x * yp;
    const double f2xz = f2x * zp;
    const double f2yy = f2y * yp;
    const double f2yz = f2y * zp;
    const double f2zz = f2z * zp;

    m[0][0] = 1.0 - (f2yy + f2zz);
    m[0][1] = f2xy - f2zw;
    m[0][2] = f2xz + f2yw;
    m[0][3] = 0.0;
    m[1][0] = f2xy + f2zw;
    m[1][1] = 1.0 - (f2xx + f2zz);
    m[1][2] = f2yz - f2xw;
    m[1][3] = 0.0;
    m[2][0] = f2xz - f2yw;
    m[2][1] = f2yz + f2xw;
    m[2][2] = 1.0 - (f2xx + f2yy);
    m[2][3] = 0.0;
    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 1.0;  // 注意最后这个元素为1，参考 QMatrix4x4的构造函数QMatrix4x4(const QGenericMatrix<N, M,
                    // float> &matrix = N)的说明
}

DMatrix4x4 DQuaternion::toMat4x4() {
    double m[4][4] = {{0.0f}, {0.0f}, {0.0f}, {0.0f}};
    toMat4x4(m);
    return DMatrix4x4(m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0],
                      m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]);
}

DQuaternion DQuaternion::normalized() const {
    // Need some extra precision if the length is very small.
    double len =
        double(xp) * double(xp) + double(yp) * double(yp) + double(zp) * double(zp) + double(wp) * double(wp);
    if (MathUtil::FuzzyIsNull(len - 1.0))
        return *this;
    else if (!MathUtil::FuzzyIsNull(len))
        return *this / std::sqrt(len);
    else
        return DQuaternion(0.0, 0.0, 0.0, 0.0);
}

DQuaternion DQuaternion::fromEulerAngles(double pitch, double yaw, double roll) {
    // Algorithm from:
    // http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q60

    pitch = MathUtil::DegreeToRadian(pitch);
    yaw = MathUtil::DegreeToRadian(yaw);
    roll = MathUtil::DegreeToRadian(roll);

    return fromRadians(pitch, yaw, roll);
}

DQuaternion DQuaternion::fromRadians(double pitch, double yaw, double roll) {
    pitch *= 0.5;
    yaw *= 0.5;
    roll *= 0.5;

    const double c1 = std::cos(yaw);
    const double s1 = std::sin(yaw);
    const double c2 = std::cos(roll);
    const double s2 = std::sin(roll);
    const double c3 = std::cos(pitch);
    const double s3 = std::sin(pitch);
    const double c1c2 = c1 * c2;
    const double s1s2 = s1 * s2;

    const double w = c1c2 * c3 + s1s2 * s3;
    const double x = c1c2 * s3 + s1s2 * c3;
    const double y = s1 * c2 * c3 - c1 * s2 * s3;
    const double z = c1 * s2 * c3 - s1 * c2 * s3;

    return DQuaternion(w, x, y, z);
}

DQuaternion DQuaternion::rotationTo(const DVector3D& from, const DVector3D& to) {
    // Based on Stan Melax's article in Game Programming Gems

    const DVector3D v0(from.normalized());
    const DVector3D v1(to.normalized());

    double d = DVector3D::dotProduct(v0, v1) + 1.0;

    // if dest vector is close to the inverse of source vector, ANY axis of rotation is valid
    if (MathUtil::FuzzyIsNull(d)) {
        DVector3D axis = DVector3D::crossProduct(DVector3D(1.0, 0.0, 0.0), v0);
        if (MathUtil::FuzzyIsNull(axis.lengthSquared()))
            axis = DVector3D::crossProduct(DVector3D(0.0, 1.0, 0.0), v0);
        axis.normalize();

        // same as QQuaternion::fromAxisAndAngle(axis, 180.0f)
        return DQuaternion(0.0, axis.x(), axis.y(), axis.z());
    }

    d = std::sqrt(2.0 * d);
    const DVector3D axis(DVector3D::crossProduct(v0, v1) / d);

    return DQuaternion(d * 0.5, axis).normalized();
}

void DQuaternion::getAxisAndAngle(DVector3D* axis, float* angle) const {
    const double length = std::hypot(xp, yp, zp);
    if (!MathUtil::FuzzyIsNull(length)) {
        *axis = DVector3D(xp, yp, zp);
        if (MathUtil::FuzzyIsEqual(length, 1.0)) {
        } else {
            (*axis) /= length;
        }
        *angle = MathUtil::RadianToDegree(2.0 * std::acos(wp));
    } else {
        // angle is 0 (mod 2*pi), so any axis will fit
        *axis = DVector3D(0., 0., 0.);
        *angle = 0.;
    }
}

DQuaternion DQuaternion::fromAxisAndAngle(const DVector3D& axis, float angle) {
    // Algorithm from:
    // http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q56
    // We normalize the result just in case the values are close
    // to zero, as suggested in the above FAQ.
    double a = MathUtil::DegreeToRadian(angle / 2.0f);
    double s = std::sin(a);
    double c = std::cos(a);
    DVector3D ax = axis.normalized();
    return DQuaternion(c, ax.x() * s, ax.y() * s, ax.z() * s).normalized();
}

DMatrix3x3 DQuaternion::toRotationMatrix() const {
    DMatrix3x3 rot3x3(Uninitialized);

    const double f2x = xp + xp;
    const double f2y = yp + yp;
    const double f2z = zp + zp;
    const double f2xw = f2x * wp;
    const double f2yw = f2y * wp;
    const double f2zw = f2z * wp;
    const double f2xx = f2x * xp;
    const double f2xy = f2x * yp;
    const double f2xz = f2x * zp;
    const double f2yy = f2y * yp;
    const double f2yz = f2y * zp;
    const double f2zz = f2z * zp;

    rot3x3(0, 0) = 1.0f - (f2yy + f2zz);
    rot3x3(0, 1) = f2xy - f2zw;
    rot3x3(0, 2) = f2xz + f2yw;
    rot3x3(1, 0) = f2xy + f2zw;
    rot3x3(1, 1) = 1.0f - (f2xx + f2zz);
    rot3x3(1, 2) = f2yz - f2xw;
    rot3x3(2, 0) = f2xz - f2yw;
    rot3x3(2, 1) = f2yz + f2xw;
    rot3x3(2, 2) = 1.0f - (f2xx + f2yy);

    return rot3x3;
}

DQuaternion DQuaternion::fromRotationMatrix(const DMatrix3x3& rot3x3) {
    // Algorithm from:
    // http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q55

    double scalar;
    double axis[3];

    const double trace = rot3x3(0, 0) + rot3x3(1, 1) + rot3x3(2, 2);
    if (trace > 0.00000001f) {
        const double s = 2.0f * std::sqrt(trace + 1.0f);
        scalar = 0.25f * s;
        axis[0] = (rot3x3(2, 1) - rot3x3(1, 2)) / s;
        axis[1] = (rot3x3(0, 2) - rot3x3(2, 0)) / s;
        axis[2] = (rot3x3(1, 0) - rot3x3(0, 1)) / s;
    } else {
        static int s_next[3] = {1, 2, 0};
        int i = 0;
        if (rot3x3(1, 1) > rot3x3(0, 0)) i = 1;
        if (rot3x3(2, 2) > rot3x3(i, i)) i = 2;
        int j = s_next[i];
        int k = s_next[j];

        const double s = 2.0f * std::sqrt(rot3x3(i, i) - rot3x3(j, j) - rot3x3(k, k) + 1.0f);
        axis[i] = 0.25f * s;
        scalar = (rot3x3(k, j) - rot3x3(j, k)) / s;
        axis[j] = (rot3x3(j, i) + rot3x3(i, j)) / s;
        axis[k] = (rot3x3(k, i) + rot3x3(i, k)) / s;
    }

    return DQuaternion(scalar, axis[0], axis[1], axis[2]);
}
}  // namespace geometry
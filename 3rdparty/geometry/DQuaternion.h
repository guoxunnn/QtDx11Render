#ifndef _DQuaternion_H_
#define _DQuaternion_H_

#include "geometry/DMatrix4x4.h"
#if defined(_WIN32) || defined(_WIN32_) || defined(WIN32) || defined(_WIN64_) || defined(WIN64) || \
    defined(_WIN64)
#include <immintrin.h>
#endif
namespace geometry {
class DQuaternion {
public:
    DQuaternion();

    DQuaternion(double scalar, double xpos, double ypos, double zpos);

    DQuaternion(double scalar, const DVector3D &vector);

    DQuaternion normalized() const;

    DQuaternion conjugated() const;

    DVector3D vector() const;

    void setToIdentity();

    DVector3D rotatedVector(const DVector3D &vector) const;

    void toMat3x3(double m[3][3]);
    void toMat4x4(double m[4][4]);
    DMatrix4x4 toMat4x4();
    inline double x() const;
    inline double y() const;
    inline double z() const;
    inline double scalar() const;

    inline DQuaternion inverted() const;

    static DQuaternion fromEulerAngles(double pitch, double yaw, double roll);
    static DQuaternion fromRadians(double pitch, double yaw, double roll);

    static DQuaternion rotationTo(const DVector3D &from, const DVector3D &to);

    friend inline const DQuaternion operator*(const DQuaternion &q1, const DQuaternion &q2);
    friend inline const DQuaternion operator/(const DQuaternion &quaternion, double divisor);
    DQuaternion &operator*=(const DQuaternion &quaternion);

    void getAxisAndAngle(DVector3D *axis, float *angle) const;
    static DQuaternion fromAxisAndAngle(const DVector3D &axis, float angle);

    DMatrix3x3 toRotationMatrix() const;
    static DQuaternion fromRotationMatrix(const DMatrix3x3 &rot3x3);
private:
    double wp, xp, yp, zp;
};

inline DQuaternion::DQuaternion() : wp(1.0f), xp(0.0f), yp(0.0f), zp(0.0f) {}

inline DQuaternion DQuaternion::conjugated() const { return DQuaternion(wp, -xp, -yp, -zp); }

inline DVector3D DQuaternion::vector() const { return DVector3D(xp, yp, zp); }

double DQuaternion::x() const { return this->xp; }

double DQuaternion::y() const { return this->yp; }

double DQuaternion::z() const { return this->zp; }

double DQuaternion::scalar() const { return this->wp; }

inline DQuaternion DQuaternion::inverted() const {
    // Need some extra precision if the length is very small.
    double len = (wp) * (wp) + (xp) * (xp) + (yp) * (yp) + (zp) * (zp);
    if (!MathUtil::FuzzyIsNull(len)) return DQuaternion(((wp) / len), (-xp) / len, (-yp) / len, (-zp) / len);
    return DQuaternion(0.0, 0.0, 0.0, 0.0);
}

inline DQuaternion::DQuaternion(double aScalar, double xpos, double ypos, double zpos)
    : wp(aScalar), xp(xpos), yp(ypos), zp(zpos) {}

inline DQuaternion::DQuaternion(double aScalar, const DVector3D &aVector)
    : wp(aScalar), xp(aVector.x()), yp(aVector.y()), zp(aVector.z()) {}

inline const DQuaternion operator*(const DQuaternion &q1, const DQuaternion &q2) {
    // 用户可能会用ARM跑X86程序
/*#ifdef _INCLUDED_IMM// x86_64 only support
    alignas(32) double datas[16] = { q1.wp, -q1.xp, -q1.yp, -q1.zp, q1.xp, q1.wp, -q1.zp, q1.yp,
        q1.yp, q1.zp, q1.wp, -q1.xp, q1.zp, -q1.yp, q1.xp, q1.wp };
    __m256d vec_attr = _mm256_loadu_pd((double*)&q2);
    __m256d vec0 = _mm256_load_pd(datas);
    vec0 = _mm256_mul_pd(vec0, vec_attr);
    __m256d vec1 = _mm256_load_pd(&datas[4]);
    vec1 = _mm256_mul_pd(vec1, vec_attr);
    __m256d vec2 = _mm256_load_pd(&datas[8]);
    vec2 = _mm256_mul_pd(vec2, vec_attr);
    __m256d vec3 = _mm256_load_pd(&datas[12]);
    vec3 = _mm256_mul_pd(vec3, vec_attr);

    vec0 = _mm256_hadd_pd(vec0, vec0);
    _mm256_store_pd(&datas[0], vec0);
    vec1 = _mm256_hadd_pd(vec1, vec1);
    _mm256_store_pd(&datas[4], vec1);
    vec2 = _mm256_hadd_pd(vec2, vec2);
    _mm256_store_pd(&datas[8], vec2);
    vec3 = _mm256_hadd_pd(vec3, vec3);
    _mm256_store_pd(&datas[12], vec3);
    return DQuaternion(datas[0] + datas[2], datas[4] + datas[6], datas[8] + datas[10], datas[12] + datas[14]);
#else*/
    double yy = (q1.wp - q1.yp) * (q2.wp + q2.zp);
    double zz = (q1.wp + q1.yp) * (q2.wp - q2.zp);
    double ww = (q1.zp + q1.xp) * (q2.xp + q2.yp);
    double xx = ww + yy + zz;
    double qq = 0.5 * (xx + (q1.zp - q1.xp) * (q2.xp - q2.yp));

    double w = qq - ww + (q1.zp - q1.yp) * (q2.yp - q2.zp);
    double x = qq - xx + (q1.xp + q1.wp) * (q2.xp + q2.wp);
    double y = qq - yy + (q1.wp - q1.xp) * (q2.yp + q2.zp);
    double z = qq - zz + (q1.zp + q1.yp) * (q2.wp - q2.xp);

    return DQuaternion(w, x, y, z);
//#endif
}

inline const DQuaternion operator/(const DQuaternion &quaternion, double divisor) {
    return DQuaternion(quaternion.wp / divisor, quaternion.xp / divisor, quaternion.yp / divisor,
                       quaternion.zp / divisor);
}

inline DQuaternion &DQuaternion::operator*=(const DQuaternion &quaternion) {
    *this = *this * quaternion;
    return *this;
}
}  // namespace geometry
#endif  // _DQuaternion_H_
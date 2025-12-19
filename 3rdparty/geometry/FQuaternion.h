//#ifndef _FQuaternion_H_
//#define _FQuaternion_H_

//#include "geometry/FVector3D.h"
//#include "geometry/GenericMatrix.h"

//namespace geometry {
//class FQuaternion {
//public:
//    FQuaternion();

//    FQuaternion(float scalar, float xpos, float ypos, float zpos);

//    FQuaternion(float scalar, const FVector3D &vector);

//    FQuaternion normalized() const;

//    FQuaternion conjugated() const;

//    FVector3D vector() const;

//    void setToIdentity();

//    FVector3D rotatedVector(const FVector3D &vector) const;

//    void toMat3x3(float m[3][3]);
//    void toMat4x4(float m[4][4]);

//    inline float x() const;
//    inline float y() const;
//    inline float z() const;
//    inline float scalar() const;

//    inline FQuaternion inverted() const;

//    void getEulerAngles(float *pitch, float *yaw, float *roll) const;

//    static FQuaternion fromEulerAngles(float pitch, float yaw, float roll);
//    static FQuaternion fromRadians(float pitch, float yaw, float roll);

//    static FQuaternion rotationTo(const FVector3D &from, const FVector3D &to);

//    friend inline const FQuaternion operator*(const FQuaternion &q1, const FQuaternion &q2);
//    friend inline const FQuaternion operator/(const FQuaternion &quaternion, float divisor);
//    FQuaternion &operator*=(const FQuaternion &quaternion);

//    void getAxisAndAngle(FVector3D *axis, float *angle) const;
//    static FQuaternion fromAxisAndAngle(const FVector3D &axis, float angle);

//    FMatrix3x3 toRotationMatrix() const;
//    static FQuaternion fromRotationMatrix(const FMatrix3x3 &rot3x3);
//    static FQuaternion MulQuat(const FQuaternion &q1, const FQuaternion &q2);

//    union {
//        float mArr[4];
//        struct {
//            float wp, xp, yp, zp;
//        };
//    };
//    // float wp, xp, yp, zp;
//};

//inline FQuaternion::FQuaternion() : wp(1.0f), xp(0.0f), yp(0.0f), zp(0.0f) {}

//inline FQuaternion FQuaternion::conjugated() const { return FQuaternion(wp, -xp, -yp, -zp); }

//inline FVector3D FQuaternion::vector() const { return FVector3D(xp, yp, zp); }

//float FQuaternion::x() const { return this->xp; }

//float FQuaternion::y() const { return this->yp; }

//float FQuaternion::z() const { return this->zp; }

//float FQuaternion::scalar() const { return this->wp; }

//inline FQuaternion FQuaternion::inverted() const {
//    // Need some extra precision if the length is very small.
//    float len = (wp) * (wp) + (xp) * (xp) + (yp) * (yp) + (zp) * (zp);
//    if (!MathUtil::FuzzyIsNull(len)) return FQuaternion(((wp) / len), (-xp) / len, (-yp) / len, (-zp) / len);
//    return FQuaternion(0.0, 0.0, 0.0, 0.0);
//}

//inline FQuaternion::FQuaternion(float aScalar, float xpos, float ypos, float zpos)
//    : wp(aScalar), xp(xpos), yp(ypos), zp(zpos) {}

//inline FQuaternion::FQuaternion(float aScalar, const FVector3D &aVector)
//    : wp(aScalar), xp(aVector.x()), yp(aVector.y()), zp(aVector.z()) {}

//const FQuaternion operator*(const FQuaternion &q1, const FQuaternion &q2) {
//    return FQuaternion::MulQuat(q1, q2);
//}

//inline const FQuaternion operator/(const FQuaternion &quaternion, float divisor) {
//    return FQuaternion(quaternion.wp / divisor, quaternion.xp / divisor, quaternion.yp / divisor,
//                       quaternion.zp / divisor);
//}

//inline FQuaternion &FQuaternion::operator*=(const FQuaternion &quaternion) {
//    *this = *this * quaternion;
//    return *this;
//}

//}  // namespace geometry
//#endif  // _FQuaternion_H_

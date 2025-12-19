#ifndef _FVector3D_H_
#define _FVector3D_H_

/*
    使用 float 数据类型的 vector 3d
*/

#include "geometry/MathUtil.h"

namespace geometry {
class FVector2D;
class FVector4D;
class DVector3D;

class FVector3D {
public:
    FVector3D();
    FVector3D(const FVector3D &vector);
    FVector3D(float xpos, float ypos, float zpos);
    FVector3D(const FVector2D &vector, float zpos = 0);
    explicit FVector3D(const FVector4D &vector);
    explicit FVector3D(const DVector3D &vector);
    ~FVector3D() = default;

    inline float x() const;
    inline float y() const;
    inline float z() const;
    inline void setX(float xpos);
    inline void setY(float ypos);
    inline void setZ(float zpos);

    double lengthD() const {
        return double(mX) * double(mX) + double(mY) * double(mY) + double(mZ) * double(mZ);
    }

    inline float length() const {
        // Need some extra precision if the length is very small.
        double len = lengthD();
        return float(std::sqrt(len));
    }

    inline float lengthSquared() const { return mX * mX + mY * mY + mZ * mZ; }

    FVector3D normalized() const;
    void normalize();

    inline bool isNull() const;

    float distanceToPoint(const FVector3D &point) const;

    // direction是单位化的方向才可求出正确解
    float distanceToLine(const FVector3D &point, const FVector3D &direction) const;

    void toMat3x3(float m_[3][3]);
    void toMat4x4(float m_[4][4]);

    inline FVector3D &operator*=(float factor);
    inline FVector3D &operator/=(float divisor);
    inline FVector3D &operator+=(const FVector3D &vector);
    inline FVector3D &operator-=(const FVector3D &vector);
    FVector3D &operator=(const DVector3D &vector);

    friend inline bool operator==(const FVector3D &v1, const FVector3D &v2);
    friend inline bool operator!=(const FVector3D &v1, const FVector3D &v2);
    friend inline const FVector3D operator+(const FVector3D &v1, const FVector3D &v2);
    friend inline const FVector3D operator-(const FVector3D &v1, const FVector3D &v2);
    friend inline const FVector3D operator*(float factor, const FVector3D &vector);
    friend inline const FVector3D operator*(const FVector3D &vector, float factor);
    friend inline const FVector3D operator*(const FVector3D &v1, const FVector3D &v2);
    friend inline const FVector3D operator-(const FVector3D &vector);
    friend inline const FVector3D operator/(const FVector3D &vector, float divisor);

    static float dotProduct(const FVector3D &v1, const FVector3D &v2);

    // 叉积计算为矢量行列式
    //   i   j   k
    //   x0  y0  z0
    //   x1  y1  z1
    static FVector3D crossProduct(const FVector3D &v1, const FVector3D &v2);

    static FVector3D normal(const FVector3D &v1, const FVector3D &v2);
    static FVector3D normal(const FVector3D &v1, const FVector3D &v2, const FVector3D &v3);

    inline float &operator[](int index) { return mArr[index]; }
    inline float operator[](int index) const { return mArr[index]; }

    float getAngleTo(const FVector3D &v) const;
    float getAngleTo(const FVector3D &v, const FVector3D &normal) const;

private:
    union {
        float mArr[3];
        struct {
            float mX, mY, mZ;
        };
    };
};

inline float FVector3D::x() const { return mX; }
inline float FVector3D::y() const { return mY; }
inline float FVector3D::z() const { return mZ; }

inline void FVector3D::setX(float xpos) { mX = xpos; }

inline void FVector3D::setY(float ypos) { mY = ypos; }

inline void FVector3D::setZ(float zpos) { mZ = zpos; }

inline bool FVector3D::isNull() const {
    return MathUtil::IsNull(mX) && MathUtil::IsNull(mY) && MathUtil::IsNull(mZ);
}

inline FVector3D &FVector3D::operator*=(float factor) {
    mX *= factor;
    mY *= factor;
    mZ *= factor;
    return *this;
}

inline FVector3D &FVector3D::operator/=(float divisor) {
    mX /= divisor;
    mY /= divisor;
    mZ /= divisor;
    return *this;
}

inline FVector3D &FVector3D::operator+=(const FVector3D &vector) {
    mX += vector.mX;
    mY += vector.mY;
    mZ += vector.mZ;
    return *this;
}

inline FVector3D &FVector3D::operator-=(const FVector3D &vector) {
    mX -= vector.mX;
    mY -= vector.mY;
    mZ -= vector.mZ;
    return *this;
}

inline bool operator==(const FVector3D &v1, const FVector3D &v2) {
    return v1.mX == v2.mX && v1.mY == v2.mY && v1.mZ == v2.mZ;
}

inline bool operator!=(const FVector3D &v1, const FVector3D &v2) {
    return v1.mX != v2.mX || v1.mY != v2.mY || v1.mZ != v2.mZ;
}

inline const FVector3D operator+(const FVector3D &v1, const FVector3D &v2) {
    return FVector3D(v1.mX + v2.mX, v1.mY + v2.mY, v1.mZ + v2.mZ);
}

inline const FVector3D operator-(const FVector3D &v1, const FVector3D &v2) {
    return FVector3D(v1.mX - v2.mX, v1.mY - v2.mY, v1.mZ - v2.mZ);
}

inline const FVector3D operator*(float factor, const FVector3D &vector) {
    return FVector3D(vector.mX * factor, vector.mY * factor, vector.mZ * factor);
}

inline const FVector3D operator*(const FVector3D &vector, float factor) {
    return FVector3D(vector.mX * factor, vector.mY * factor, vector.mZ * factor);
}

inline const FVector3D operator*(const FVector3D &v1, const FVector3D &v2) {
    return FVector3D(v1.mX * v2.mX, v1.mY * v2.mY, v1.mZ * v2.mZ);
}

inline const FVector3D operator-(const FVector3D &vector) {
    return FVector3D(-vector.mX, -vector.mY, -vector.mZ);
}

inline const FVector3D operator/(const FVector3D &vector, float divisor) {
    return FVector3D(vector.mX / divisor, vector.mY / divisor, vector.mZ / divisor);
}
}  // namespace geometry
#endif  // _FVector3D_H_

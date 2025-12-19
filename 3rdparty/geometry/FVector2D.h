#ifndef _FVector2D_H_
#define _FVector2D_H_

#include "geometry/MathUtil.h"

/*
    使用 float 数据类型的 vector 2d
*/
namespace geometry {
class FVector3D;

class FVector2D {
public:
    FVector2D();
    FVector2D(float xpos, float ypos);

    explicit FVector2D(const FVector3D &vector);
    inline float x() const;
    inline float y() const;

    inline void setX(float xpos);
    inline void setY(float ypos);

    inline float length() const { return std::sqrt(mX * mX + mY * mY); }
    inline float lengthSquared() const { return mX * mX + mY * mY; }

    FVector2D normalized() const;
    void normalize();

    inline bool isNull() const;

    inline float distanceToPoint(const FVector2D &point) const { return (*this - point).length(); }

    // direction是单位化的方向才可求出正确解
    float distanceToLine(const FVector2D &point, const FVector2D &direction) const;

    inline FVector2D &operator*=(float factor);
    inline FVector2D &operator/=(float divisor);
    inline FVector2D &operator+=(const FVector2D &vector);
    inline FVector2D &operator-=(const FVector2D &vector);

    friend inline bool operator==(const FVector2D &v1, const FVector2D &v2);
    friend inline bool operator!=(const FVector2D &v1, const FVector2D &v2);
    friend inline const FVector2D operator+(const FVector2D &v1, const FVector2D &v2);
    friend inline const FVector2D operator-(const FVector2D &v1, const FVector2D &v2);
    friend inline const FVector2D operator*(float factor, const FVector2D &vector);
    friend inline const FVector2D operator*(const FVector2D &vector, float factor);
    friend inline const FVector2D operator-(const FVector2D &vector);
    friend inline const FVector2D operator/(const FVector2D &vector, float divisor);

    static inline float dotProduct(const FVector2D &v1, const FVector2D &v2) {
        return v1.mX * v2.mX + v1.mY * v2.mY;
    }

    static inline float crossProduct(const FVector2D &v1, const FVector2D &v2) {
        return v1.mX * v2.mY - v1.mY * v2.mX;
    }

    inline float &operator[](int index) { return mArr[index]; }
    inline float operator[](int index) const { return mArr[index]; }

private:
    union {
        float mArr[2];
        struct {
            float mX, mY;
        };
    };
};

inline float FVector2D::x() const { return mX; }

inline float FVector2D::y() const { return mY; }

inline void FVector2D::setX(float xpos) { mX = xpos; }

inline void FVector2D::setY(float ypos) { mY = ypos; }

inline bool FVector2D::isNull() const { return MathUtil::IsNull(mX) && MathUtil::IsNull(mY); }

inline FVector2D &FVector2D::operator*=(float factor) {
    mX *= factor;
    mY *= factor;
    return *this;
}

inline FVector2D &FVector2D::operator/=(float factor) {
    mX /= factor;
    mY /= factor;
    return *this;
}

inline FVector2D &FVector2D::operator+=(const FVector2D &vector) {
    mX += vector.mX;
    mY += vector.mY;
    return *this;
}

inline FVector2D &FVector2D::operator-=(const FVector2D &vector) {
    mX -= vector.mX;
    mY -= vector.mY;
    return *this;
}

inline bool operator==(const FVector2D &v1, const FVector2D &v2) { return v1.mX == v2.mX && v1.mY == v2.mY; }

inline bool operator!=(const FVector2D &v1, const FVector2D &v2) { return v1.mX != v2.mX || v1.mY != v2.mY; }

inline const FVector2D operator+(const FVector2D &v1, const FVector2D &v2) {
    return FVector2D(v1.mX + v2.mX, v1.mY + v2.mY);
}

inline const FVector2D operator-(const FVector2D &v1, const FVector2D &v2) {
    return FVector2D(v1.mX - v2.mX, v1.mY - v2.mY);
}

inline const FVector2D operator*(float factor, const FVector2D &vector) {
    return FVector2D(vector.mX * factor, vector.mY * factor);
}

inline const FVector2D operator*(const FVector2D &vector, float factor) {
    return FVector2D(vector.mX * factor, vector.mY * factor);
}

inline const FVector2D operator-(const FVector2D &vector) { return FVector2D(-vector.mX, -vector.mY); }

inline const FVector2D operator/(const FVector2D &vector, float divisor) {
    return FVector2D(vector.mX / divisor, vector.mY / divisor);
}
}  // namespace geometry
#endif  //_FVector2D_H_

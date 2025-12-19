#ifndef _DVector2D_H_
#define _DVector2D_H_

#include "geometry/FVector2D.h"
namespace geometry {
class DVector3D;

class DVector2D {
public:
    DVector2D();
    DVector2D(double xpos, double ypos);

    explicit DVector2D(const DVector3D &vector);
    inline double x() const;
    inline double y() const;

    inline void setX(double x);
    inline void setY(double y);

    inline double length() const { return std::sqrt(mX * mX + mY * mY); }

    inline double lengthSquared() const { return mX * mX + mY * mY; }

    DVector2D normalized() const;
    void normalize();

    inline bool isNull() const;
    inline double distanceToPoint(const DVector2D& point) const { return (*this - point).length(); }

    // direction是单位化的方向才可求出正确解
    double distanceToLine(const DVector2D &point, const DVector2D &direction) const;

    inline DVector2D &operator*=(double factor);
    inline DVector2D &operator/=(double divisor);
    inline DVector2D &operator+=(const DVector2D &vector);
    inline DVector2D &operator-=(const DVector2D &vector);

    friend inline bool operator==(const DVector2D &v1, const DVector2D &v2);
    friend inline bool operator!=(const DVector2D &v1, const DVector2D &v2);
    friend inline const DVector2D operator+(const DVector2D &v1, const DVector2D &v2);
    friend inline const DVector2D operator-(const DVector2D &v1, const DVector2D &v2);
    friend inline const DVector2D operator*(double factor, const DVector2D &vector);
    friend inline const DVector2D operator*(const DVector2D &vector, double factor);
    friend inline const DVector2D operator-(const DVector2D &vector);
    friend inline const DVector2D operator/(const DVector2D &vector, double divisor);

    static inline double dotProduct(const DVector2D &v1, const DVector2D &v2) {
        return v1.mX * v2.mX + v1.mY * v2.mY;
    }

    static inline double crossProduct(const DVector2D &v1, const DVector2D &v2) {
        return v1.mX * v2.mY - v1.mY * v2.mX;
    }

    inline FVector2D toFVector2D() const;

    inline double &operator[](int index) { return mArr[index]; }
    inline double operator[](int index) const { return mArr[index]; }

private:
    union {
        double mArr[2];
        struct {
            double mX, mY;
        };
    };
};

inline double DVector2D::x() const { return mX; }

inline double DVector2D::y() const { return mY; }

inline void DVector2D::setX(double x) { mX = x; }

inline void DVector2D::setY(double y) { mY = y; }

inline bool DVector2D::isNull() const { return MathUtil::IsNull(mX) && MathUtil::IsNull(mY); }

inline DVector2D &DVector2D::operator*=(double factor) {
    mX *= factor;
    mY *= factor;
    return *this;
}

inline DVector2D &DVector2D::operator/=(double factor) {
    mX /= factor;
    mY /= factor;
    return *this;
}

inline DVector2D &DVector2D::operator+=(const DVector2D &vector) {
    mX += vector.mX;
    mY += vector.mY;
    return *this;
}

inline DVector2D &DVector2D::operator-=(const DVector2D &vector) {
    mX -= vector.mX;
    mY -= vector.mY;
    return *this;
}

inline bool operator==(const DVector2D &v1, const DVector2D &v2) { return v1.mX == v2.mX && v1.mY == v2.mY; }

inline bool operator!=(const DVector2D &v1, const DVector2D &v2) { return v1.mX != v2.mX || v1.mY != v2.mY; }

inline const DVector2D operator+(const DVector2D &v1, const DVector2D &v2) {
    return DVector2D(v1.mX + v2.mX, v1.mY + v2.mY);
}

inline const DVector2D operator-(const DVector2D &v1, const DVector2D &v2) {
    return DVector2D(v1.mX - v2.mX, v1.mY - v2.mY);
}

inline const DVector2D operator*(double factor, const DVector2D &vector) {
    return DVector2D(vector.mX * factor, vector.mY * factor);
}

inline const DVector2D operator*(const DVector2D &vector, double factor) {
    return DVector2D(vector.mX * factor, vector.mY * factor);
}

inline const DVector2D operator-(const DVector2D &vector) { return DVector2D(-vector.mX, -vector.mY); }

inline const DVector2D operator/(const DVector2D &vector, double divisor) {
    return DVector2D(vector.mX / divisor, vector.mY / divisor);
}

inline FVector2D DVector2D::toFVector2D() const { return FVector2D((float)mX, (float)mY); }
}  // namespace geometry
#endif  // _DVector2D_H_

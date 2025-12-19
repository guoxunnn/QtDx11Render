#ifndef _DVector3D_H_
#define _DVector3D_H_

#include "geometry/DVector2D.h"
namespace geometry {
class FVector3D;
class DVector3D {
public:
    DVector3D();
    DVector3D(double xpos, double ypos, double zpos);
    DVector3D(const DVector2D &vector, double zpos);
    explicit DVector3D(const FVector3D &vector);
    inline double x() const;
    inline double y() const;
    inline double z() const;
    inline void setX(double x);
    inline void setY(double y);
    inline void setZ(double z);

    inline double length() const { return std::sqrt(mX * mX + mY * mY + mZ * mZ); }
    inline double lengthSquared() const { return mX * mX + mY * mY + mZ * mZ; }

    DVector3D normalized() const;
    void normalize();

    inline bool isNull() const;
    inline double distanceToPoint(const DVector3D &point) const { return (*this - point).length(); }

    // direction是单位化的方向才可求出正确解
    double distanceToLine(const DVector3D &point, const DVector3D &direction) const;

    void toMat3x3(double m_[3][3]);
    void toMat4x4(double m_[4][4]);

    inline DVector3D &operator*=(double factor);
    inline DVector3D &operator/=(double divisor);
    inline DVector3D &operator+=(const DVector3D &vector);
    inline DVector3D &operator-=(const DVector3D &vector);
    DVector3D &operator=(const FVector3D &vector);

    friend inline bool operator==(const DVector3D &v1, const DVector3D &v2);
    friend inline bool operator!=(const DVector3D &v1, const DVector3D &v2);
    friend inline const DVector3D operator+(const DVector3D &v1, const DVector3D &v2);
    friend inline const DVector3D operator-(const DVector3D &v1, const DVector3D &v2);
    friend inline const DVector3D operator*(double factor, const DVector3D &vector);
    friend inline const DVector3D operator*(const DVector3D &vector, double factor);
    friend inline const DVector3D operator*(const DVector3D &v1, const DVector3D &v2);
    friend inline const DVector3D operator-(const DVector3D &vector);
    friend inline const DVector3D operator/(const DVector3D &vector, double divisor);

    static inline double dotProduct(const DVector3D &v1, const DVector3D &v2) {
        return v1.mX * v2.mX + v1.mY * v2.mY + v1.mZ * v2.mZ;
    }

    // 叉积计算为矢量行列式
    //   i   j   k
    //   x0  y0  z0
    //   x1  y1  z1

    static inline DVector3D crossProduct(const DVector3D &v1, const DVector3D &v2) {
        return DVector3D(v1.mY * v2.mZ - v1.mZ * v2.mY, v1.mZ * v2.mX - v1.mX * v2.mZ,
                         v1.mX * v2.mY - v1.mY * v2.mX);
    }

    static inline DVector3D normal(const DVector3D &v1, const DVector3D &v2) {
        return crossProduct(v1, v2).normalized();
    }

    static inline DVector3D normal(const DVector3D &v1, const DVector3D &v2, const DVector3D &v3) {
        return crossProduct((v2 - v1), (v3 - v1)).normalized();
    }

    FVector3D toFVector3D() const;

    inline double &operator[](int index) { return mArr[index]; }
    inline double operator[](int index) const { return mArr[index]; }

    float getAngleTo(const DVector3D &v) const;
    float getAngleTo(const DVector3D &v, const DVector3D &normal) const;

private:
    union {
        double mArr[3];
        struct {
            double mX, mY, mZ;
        };
    };
};

inline double DVector3D::x() const { return mX; }
inline double DVector3D::y() const { return mY; }
inline double DVector3D::z() const { return mZ; }

inline void DVector3D::setX(double x) { mX = x; }

inline void DVector3D::setY(double y) { mY = y; }

inline void DVector3D::setZ(double z) { mZ = z; }

inline bool DVector3D::isNull() const {
    return MathUtil::IsNull(mX) && MathUtil::IsNull(mY) && MathUtil::IsNull(mZ);
}

inline DVector3D &DVector3D::operator*=(double factor) {
    mX *= factor;
    mY *= factor;
    mZ *= factor;
    return *this;
}

inline DVector3D &DVector3D::operator/=(double divisor) {
    mX /= divisor;
    mY /= divisor;
    mZ /= divisor;
    return *this;
}

inline DVector3D &DVector3D::operator+=(const DVector3D &vector) {
    mX += vector.mX;
    mY += vector.mY;
    mZ += vector.mZ;
    return *this;
}

inline DVector3D &DVector3D::operator-=(const DVector3D &vector) {
    mX -= vector.mX;
    mY -= vector.mY;
    mZ -= vector.mZ;
    return *this;
}

inline bool operator==(const DVector3D &v1, const DVector3D &v2) {
    return v1.mX == v2.mX && v1.mY == v2.mY && v1.mZ == v2.mZ;
}

inline bool operator!=(const DVector3D &v1, const DVector3D &v2) {
    return v1.mX != v2.mX || v1.mY != v2.mY || v1.mZ != v2.mZ;
}

inline const DVector3D operator+(const DVector3D &v1, const DVector3D &v2) {
    return DVector3D(v1.mX + v2.mX, v1.mY + v2.mY, v1.mZ + v2.mZ);
}

inline const DVector3D operator-(const DVector3D &v1, const DVector3D &v2) {
    return DVector3D(v1.mX - v2.mX, v1.mY - v2.mY, v1.mZ - v2.mZ);
}

inline const DVector3D operator*(double factor, const DVector3D &vector) {
    return DVector3D(vector.mX * factor, vector.mY * factor, vector.mZ * factor);
}

inline const DVector3D operator*(const DVector3D &vector, double factor) {
    return DVector3D(vector.mX * factor, vector.mY * factor, vector.mZ * factor);
}

inline const DVector3D operator*(const DVector3D &v1, const DVector3D &v2) {
    return DVector3D(v1.mX * v2.mX, v1.mY * v2.mY, v1.mZ * v2.mZ);
}

inline const DVector3D operator-(const DVector3D &vector) {
    return DVector3D(-vector.mX, -vector.mY, -vector.mZ);
}

inline const DVector3D operator/(const DVector3D &vector, double divisor) {
    return DVector3D(vector.mX / divisor, vector.mY / divisor, vector.mZ / divisor);
}
}  // namespace geometry
#endif  // _DVector3D_H_

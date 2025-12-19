#ifndef _DVector4D_H_
#define _DVector4D_H_

#include "geometry/MathUtil.h"
#include <assert.h>
namespace geometry {
class DVector2D;
class DVector3D;
class DMatrix4x4;

class DVector4D {
public:
    constexpr DVector4D();
    explicit DVector4D(Initialization) {}
    constexpr DVector4D(double xpos, double ypos, double zpos, double wpos);

    DVector4D(const DVector2D &vector);
    DVector4D(const DVector2D &vector, double zpos, double wpos);
    DVector4D(const DVector3D &vector);
    DVector4D(const DVector3D &vector, double wpos);

    bool isNull() const;

    constexpr double x() const;
    constexpr double y() const;
    constexpr double z() const;
    constexpr double w() const;

    void setX(double x);
    void setY(double y);
    void setZ(double z);
    void setW(double w);

    double &operator[](int i);
    double operator[](int i) const;

    double length() const;
    double lengthSquared() const;  // In Qt 6 convert to inline and constexpr

    DVector4D normalized() const;
    void normalize();

    DVector4D &operator+=(const DVector4D &vector);
    DVector4D &operator-=(const DVector4D &vector);
    DVector4D &operator*=(double factor);
    DVector4D &operator*=(const DVector4D &vector);
    DVector4D &operator/=(double divisor);
    inline DVector4D &operator/=(const DVector4D &vector);

    static double dotProduct(const DVector4D &v1,
                             const DVector4D &v2);  // In Qt 6 convert to inline and constexpr

    constexpr friend inline bool operator==(const DVector4D &v1, const DVector4D &v2);
    constexpr friend inline bool operator!=(const DVector4D &v1, const DVector4D &v2);
    constexpr friend inline const DVector4D operator+(const DVector4D &v1, const DVector4D &v2);
    constexpr friend inline const DVector4D operator-(const DVector4D &v1, const DVector4D &v2);
    constexpr friend inline const DVector4D operator*(double factor, const DVector4D &vector);
    constexpr friend inline const DVector4D operator*(const DVector4D &vector, double factor);
    constexpr friend inline const DVector4D operator*(const DVector4D &v1, const DVector4D &v2);
    constexpr friend inline const DVector4D operator-(const DVector4D &vector);
    constexpr friend inline const DVector4D operator/(const DVector4D &vector, double divisor);
    constexpr friend inline const DVector4D operator/(const DVector4D &vector, const DVector4D &divisor);

    friend inline bool fuzzyCompare(const DVector4D &v1, const DVector4D &v2);

    DVector2D toVector2D() const;
    DVector2D toVector2DAffine() const;

    DVector3D toVector3D() const;
    DVector3D toVector3DAffine() const;

private:
    double v[4];

    friend class DVector2D;
    friend class DVector3D;
    friend DVector4D operator*(const DVector4D &vector, const DMatrix4x4 &matrix);
    friend DVector4D operator*(const DMatrix4x4 &matrix, const DVector4D &vector);
};

constexpr inline DVector4D::DVector4D() : v{0.0f, 0.0f, 0.0f, 0.0f} {}

constexpr inline DVector4D::DVector4D(double xpos, double ypos, double zpos, double wpos)
    : v{xpos, ypos, zpos, wpos} {}

inline bool DVector4D::isNull() const {
    return MathUtil::IsNull(v[0]) && MathUtil::IsNull(v[1]) && MathUtil::IsNull(v[2]) &&
           MathUtil::IsNull(v[3]);
}

constexpr inline double DVector4D::x() const { return v[0]; }
constexpr inline double DVector4D::y() const { return v[1]; }
constexpr inline double DVector4D::z() const { return v[2]; }
constexpr inline double DVector4D::w() const { return v[3]; }

inline void DVector4D::setX(double aX) { v[0] = aX; }
inline void DVector4D::setY(double aY) { v[1] = aY; }
inline void DVector4D::setZ(double aZ) { v[2] = aZ; }
inline void DVector4D::setW(double aW) { v[3] = aW; }

inline double &DVector4D::operator[](int i) {
    assert(uint32_t(i) < 4u);
    return v[i];
}

inline double DVector4D::operator[](int i) const {
    assert(uint32_t(i) < 4u);
    return v[i];
}

inline DVector4D &DVector4D::operator+=(const DVector4D &vector) {
    v[0] += vector.v[0];
    v[1] += vector.v[1];
    v[2] += vector.v[2];
    v[3] += vector.v[3];
    return *this;
}

inline DVector4D &DVector4D::operator-=(const DVector4D &vector) {
    v[0] -= vector.v[0];
    v[1] -= vector.v[1];
    v[2] -= vector.v[2];
    v[3] -= vector.v[3];
    return *this;
}

inline DVector4D &DVector4D::operator*=(double factor) {
    v[0] *= factor;
    v[1] *= factor;
    v[2] *= factor;
    v[3] *= factor;
    return *this;
}

inline DVector4D &DVector4D::operator*=(const DVector4D &vector) {
    v[0] *= vector.v[0];
    v[1] *= vector.v[1];
    v[2] *= vector.v[2];
    v[3] *= vector.v[3];
    return *this;
}

inline DVector4D &DVector4D::operator/=(double divisor) {
    v[0] /= divisor;
    v[1] /= divisor;
    v[2] /= divisor;
    v[3] /= divisor;
    return *this;
}

inline DVector4D &DVector4D::operator/=(const DVector4D &vector) {
    v[0] /= vector.v[0];
    v[1] /= vector.v[1];
    v[2] /= vector.v[2];
    v[3] /= vector.v[3];
    return *this;
}

constexpr inline bool operator==(const DVector4D &v1, const DVector4D &v2) {
    return v1.v[0] == v2.v[0] && v1.v[1] == v2.v[1] && v1.v[2] == v2.v[2] && v1.v[3] == v2.v[3];
}

constexpr inline bool operator!=(const DVector4D &v1, const DVector4D &v2) {
    return v1.v[0] != v2.v[0] || v1.v[1] != v2.v[1] || v1.v[2] != v2.v[2] || v1.v[3] != v2.v[3];
}

constexpr inline const DVector4D operator+(const DVector4D &v1, const DVector4D &v2) {
    return DVector4D(v1.v[0] + v2.v[0], v1.v[1] + v2.v[1], v1.v[2] + v2.v[2], v1.v[3] + v2.v[3]);
}

constexpr inline const DVector4D operator-(const DVector4D &v1, const DVector4D &v2) {
    return DVector4D(v1.v[0] - v2.v[0], v1.v[1] - v2.v[1], v1.v[2] - v2.v[2], v1.v[3] - v2.v[3]);
}

constexpr inline const DVector4D operator*(double factor, const DVector4D &vector) {
    return DVector4D(vector.v[0] * factor, vector.v[1] * factor, vector.v[2] * factor, vector.v[3] * factor);
}

constexpr inline const DVector4D operator*(const DVector4D &vector, double factor) {
    return DVector4D(vector.v[0] * factor, vector.v[1] * factor, vector.v[2] * factor, vector.v[3] * factor);
}

constexpr inline const DVector4D operator*(const DVector4D &v1, const DVector4D &v2) {
    return DVector4D(v1.v[0] * v2.v[0], v1.v[1] * v2.v[1], v1.v[2] * v2.v[2], v1.v[3] * v2.v[3]);
}

constexpr inline const DVector4D operator-(const DVector4D &vector) {
    return DVector4D(-vector.v[0], -vector.v[1], -vector.v[2], -vector.v[3]);
}

constexpr inline const DVector4D operator/(const DVector4D &vector, double divisor) {
    return DVector4D(vector.v[0] / divisor, vector.v[1] / divisor, vector.v[2] / divisor,
                     vector.v[3] / divisor);
}

constexpr inline const DVector4D operator/(const DVector4D &vector, const DVector4D &divisor) {
    return DVector4D(vector.v[0] / divisor.v[0], vector.v[1] / divisor.v[1], vector.v[2] / divisor.v[2],
                     vector.v[3] / divisor.v[3]);
}

inline bool fuzzyCompare(const DVector4D &v1, const DVector4D &v2) {
    return MathUtil::FuzzyIsEqual(v1.v[0], v2.v[0]) && MathUtil::FuzzyIsEqual(v1.v[1], v2.v[1]) &&
           MathUtil::FuzzyIsEqual(v1.v[2], v2.v[2]) && MathUtil::FuzzyIsEqual(v1.v[3], v2.v[3]);
}
}  // namespace geometry
#endif  // _DVector4D_H_
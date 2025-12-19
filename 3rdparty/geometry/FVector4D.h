#ifndef _FVector4D_H_
#define _FVector4D_H_
#include <assert.h>
#include "geometry/MathUtil.h"
namespace geometry {
class FVector2D;
class FVector3D;
class FMatrix4x4;

class FVector4D {
public:
    constexpr FVector4D();
    explicit FVector4D(Initialization) {}
    constexpr FVector4D(float xpos, float ypos, float zpos, float wpos);

    FVector4D(const FVector2D &vector);
    FVector4D(const FVector2D &vector, float zpos, float wpos);
    FVector4D(const FVector3D &vector);
    FVector4D(const FVector3D &vector, float wpos);

    bool isNull() const;

    constexpr float x() const;
    constexpr float y() const;
    constexpr float z() const;
    constexpr float w() const;

    void setX(float xpos);
    void setY(float ypos);
    void setZ(float zpos);
    void setW(float wpos);

    float &operator[](int i);
    float operator[](int i) const;

    float length() const;
    float lengthSquared() const;  // In Qt 6 convert to inline and constexpr

    FVector4D normalized() const;
    void normalize();

    FVector4D &operator+=(const FVector4D &vector);
    FVector4D &operator-=(const FVector4D &vector);
    FVector4D &operator*=(float factor);
    FVector4D &operator*=(const FVector4D &vector);
    FVector4D &operator/=(float divisor);
    inline FVector4D &operator/=(const FVector4D &vector);

    static float dotProduct(const FVector4D &v1,
                            const FVector4D &v2);  // In Qt 6 convert to inline and constexpr

    constexpr friend inline bool operator==(const FVector4D &v1, const FVector4D &v2);
    constexpr friend inline bool operator!=(const FVector4D &v1, const FVector4D &v2);
    constexpr friend inline const FVector4D operator+(const FVector4D &v1, const FVector4D &v2);
    constexpr friend inline const FVector4D operator-(const FVector4D &v1, const FVector4D &v2);
    constexpr friend inline const FVector4D operator*(float factor, const FVector4D &vector);
    constexpr friend inline const FVector4D operator*(const FVector4D &vector, float factor);
    constexpr friend inline const FVector4D operator*(const FVector4D &v1, const FVector4D &v2);
    constexpr friend inline const FVector4D operator-(const FVector4D &vector);
    constexpr friend inline const FVector4D operator/(const FVector4D &vector, float divisor);
    constexpr friend inline const FVector4D operator/(const FVector4D &vector, const FVector4D &divisor);

    friend inline bool fuzzyCompare(const FVector4D &v1, const FVector4D &v2);

    FVector2D toVector2D() const;
    FVector2D toVector2DAffine() const;

    FVector3D toVector3D() const;
    FVector3D toVector3DAffine() const;

private:
    float v[4];

    friend class FVector2D;
    friend class FVector3D;
    friend FVector4D operator*(const FVector4D &vector, const FMatrix4x4 &matrix);
    friend FVector4D operator*(const FMatrix4x4 &matrix, const FVector4D &vector);
};

constexpr inline FVector4D::FVector4D() : v{0.0f, 0.0f, 0.0f, 0.0f} {}

constexpr inline FVector4D::FVector4D(float xpos, float ypos, float zpos, float wpos)
    : v{xpos, ypos, zpos, wpos} {}

inline bool FVector4D::isNull() const {
    return MathUtil::IsNull(v[0]) && MathUtil::IsNull(v[1]) && MathUtil::IsNull(v[2]) &&
           MathUtil::IsNull(v[3]);
}

constexpr inline float FVector4D::x() const { return v[0]; }
constexpr inline float FVector4D::y() const { return v[1]; }
constexpr inline float FVector4D::z() const { return v[2]; }
constexpr inline float FVector4D::w() const { return v[3]; }

inline void FVector4D::setX(float xpos) { v[0] = xpos; }
inline void FVector4D::setY(float ypos) { v[1] = ypos; }
inline void FVector4D::setZ(float zpos) { v[2] = zpos; }
inline void FVector4D::setW(float wpos) { v[3] = wpos; }

inline float &FVector4D::operator[](int i) {
    assert(uint32_t(i) < 4u);
    return v[i];
}

inline float FVector4D::operator[](int i) const {
    assert(uint32_t(i) < 4u);
    return v[i];
}

inline FVector4D &FVector4D::operator+=(const FVector4D &vector) {
    v[0] += vector.v[0];
    v[1] += vector.v[1];
    v[2] += vector.v[2];
    v[3] += vector.v[3];
    return *this;
}

inline FVector4D &FVector4D::operator-=(const FVector4D &vector) {
    v[0] -= vector.v[0];
    v[1] -= vector.v[1];
    v[2] -= vector.v[2];
    v[3] -= vector.v[3];
    return *this;
}

inline FVector4D &FVector4D::operator*=(float factor) {
    v[0] *= factor;
    v[1] *= factor;
    v[2] *= factor;
    v[3] *= factor;
    return *this;
}

inline FVector4D &FVector4D::operator*=(const FVector4D &vector) {
    v[0] *= vector.v[0];
    v[1] *= vector.v[1];
    v[2] *= vector.v[2];
    v[3] *= vector.v[3];
    return *this;
}

inline FVector4D &FVector4D::operator/=(float divisor) {
    v[0] /= divisor;
    v[1] /= divisor;
    v[2] /= divisor;
    v[3] /= divisor;
    return *this;
}

inline FVector4D &FVector4D::operator/=(const FVector4D &vector) {
    v[0] /= vector.v[0];
    v[1] /= vector.v[1];
    v[2] /= vector.v[2];
    v[3] /= vector.v[3];
    return *this;
}

constexpr inline bool operator==(const FVector4D &v1, const FVector4D &v2) {
    return v1.v[0] == v2.v[0] && v1.v[1] == v2.v[1] && v1.v[2] == v2.v[2] && v1.v[3] == v2.v[3];
}

constexpr inline bool operator!=(const FVector4D &v1, const FVector4D &v2) {
    return v1.v[0] != v2.v[0] || v1.v[1] != v2.v[1] || v1.v[2] != v2.v[2] || v1.v[3] != v2.v[3];
}

constexpr inline const FVector4D operator+(const FVector4D &v1, const FVector4D &v2) {
    return FVector4D(v1.v[0] + v2.v[0], v1.v[1] + v2.v[1], v1.v[2] + v2.v[2], v1.v[3] + v2.v[3]);
}

constexpr inline const FVector4D operator-(const FVector4D &v1, const FVector4D &v2) {
    return FVector4D(v1.v[0] - v2.v[0], v1.v[1] - v2.v[1], v1.v[2] - v2.v[2], v1.v[3] - v2.v[3]);
}

constexpr inline const FVector4D operator*(float factor, const FVector4D &vector) {
    return FVector4D(vector.v[0] * factor, vector.v[1] * factor, vector.v[2] * factor, vector.v[3] * factor);
}

constexpr inline const FVector4D operator*(const FVector4D &vector, float factor) {
    return FVector4D(vector.v[0] * factor, vector.v[1] * factor, vector.v[2] * factor, vector.v[3] * factor);
}

constexpr inline const FVector4D operator*(const FVector4D &v1, const FVector4D &v2) {
    return FVector4D(v1.v[0] * v2.v[0], v1.v[1] * v2.v[1], v1.v[2] * v2.v[2], v1.v[3] * v2.v[3]);
}

constexpr inline const FVector4D operator-(const FVector4D &vector) {
    return FVector4D(-vector.v[0], -vector.v[1], -vector.v[2], -vector.v[3]);
}

constexpr inline const FVector4D operator/(const FVector4D &vector, float divisor) {
    return FVector4D(vector.v[0] / divisor, vector.v[1] / divisor, vector.v[2] / divisor,
                     vector.v[3] / divisor);
}

constexpr inline const FVector4D operator/(const FVector4D &vector, const FVector4D &divisor) {
    return FVector4D(vector.v[0] / divisor.v[0], vector.v[1] / divisor.v[1], vector.v[2] / divisor.v[2],
                     vector.v[3] / divisor.v[3]);
}

inline bool fuzzyCompare(const FVector4D &v1, const FVector4D &v2) {
    return MathUtil::FuzzyIsEqual(v1.v[0], v2.v[0]) && MathUtil::FuzzyIsEqual(v1.v[1], v2.v[1]) &&
           MathUtil::FuzzyIsEqual(v1.v[2], v2.v[2]) && MathUtil::FuzzyIsEqual(v1.v[3], v2.v[3]);
}
}  // namespace geometry
#endif  // _FVector4D_H_
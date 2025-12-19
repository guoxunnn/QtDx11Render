#include "FVector4D.h"

#include "FVector2D.h"
#include "FVector3D.h"
namespace geometry {
FVector4D::FVector4D(const FVector2D& vector) {
    v[0] = vector.x();
    v[1] = vector.y();
    v[2] = 0.0f;
    v[3] = 0.0f;
}

FVector4D::FVector4D(const FVector2D& vector, float zpos, float wpos) {
    v[0] = vector.x();
    v[1] = vector.y();
    v[2] = zpos;
    v[3] = wpos;
}

FVector4D::FVector4D(const FVector3D& vector) {
    v[0] = vector.x();
    v[1] = vector.y();
    v[2] = vector.z();
    v[3] = 0.0f;
}

FVector4D::FVector4D(const FVector3D& vector, float wpos) {
    v[0] = vector.x();
    v[1] = vector.y();
    v[2] = vector.z();
    v[3] = wpos;
}

float FVector4D::length() const {
    double len = double(v[0]) * double(v[0]) + double(v[1]) * double(v[1]) + double(v[2]) * double(v[2]) +
                 double(v[3]) * double(v[3]);
    return float(std::sqrt(len));
}

float FVector4D::lengthSquared() const { return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]; }

FVector4D FVector4D::normalized() const {
    double len = double(v[0]) * double(v[0]) + double(v[1]) * double(v[1]) + double(v[2]) * double(v[2]) +
                 double(v[3]) * double(v[3]);
    if (MathUtil::FuzzyIsNull(len - 1.0f)) {
        return *this;
    } else if (!MathUtil::FuzzyIsNull(len)) {
        double sqrtLen = std::sqrt(len);
        return FVector4D(float(double(v[0]) / sqrtLen), float(double(v[1]) / sqrtLen),
                         float(double(v[2]) / sqrtLen), float(double(v[3]) / sqrtLen));
    } else {
        return FVector4D();
    }
}

void FVector4D::normalize() {
    double len = double(v[0]) * double(v[0]) + double(v[1]) * double(v[1]) + double(v[2]) * double(v[2]) +
                 double(v[3]) * double(v[3]);
    if (MathUtil::FuzzyIsNull(len - 1.0f) || MathUtil::FuzzyIsNull(len)) return;

    len = std::sqrt(len);

    v[0] = float(double(v[0]) / len);
    v[1] = float(double(v[1]) / len);
    v[2] = float(double(v[2]) / len);
    v[3] = float(double(v[3]) / len);
}

FVector2D FVector4D::toVector2D() const { return FVector2D(v[0], v[1]); }

FVector2D FVector4D::toVector2DAffine() const {
    if (MathUtil::IsNull(v[3])) { return FVector2D(); }
    return FVector2D(v[0] / v[3], v[1] / v[3]);
}

FVector3D FVector4D::toVector3D() const { return FVector3D(v[0], v[1], v[2]); }

FVector3D FVector4D::toVector3DAffine() const {
    if (MathUtil::IsNull(v[3])) { return FVector3D(); }
    return FVector3D(v[0] / v[3], v[1] / v[3], v[2] / v[3]);
}
}  // namespace geometry

#include "DVector4D.h"

#include "DVector2D.h"
#include "DVector3D.h"
namespace geometry {
DVector4D::DVector4D(const DVector2D& vector) {
    v[0] = vector.x();
    v[1] = vector.y();
    v[2] = 0.0f;
    v[3] = 0.0f;
}

DVector4D::DVector4D(const DVector2D& vector, double zpos, double wpos) {
    v[0] = vector.x();
    v[1] = vector.y();
    v[2] = zpos;
    v[3] = wpos;
}

DVector4D::DVector4D(const DVector3D& vector) {
    v[0] = vector.x();
    v[1] = vector.y();
    v[2] = vector.z();
    v[3] = 0.0f;
}

DVector4D::DVector4D(const DVector3D& vector, double wpos) {
    v[0] = vector.x();
    v[1] = vector.y();
    v[2] = vector.z();
    v[3] = wpos;
}

double DVector4D::length() const {
    double len = double(v[0]) * double(v[0]) + double(v[1]) * double(v[1]) + double(v[2]) * double(v[2]) +
                 double(v[3]) * double(v[3]);
    return double(std::sqrt(len));
}

double DVector4D::lengthSquared() const { return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]; }

DVector4D DVector4D::normalized() const {
    double len = double(v[0]) * double(v[0]) + double(v[1]) * double(v[1]) + double(v[2]) * double(v[2]) +
                 double(v[3]) * double(v[3]);
    if (MathUtil::FuzzyIsNull(len - 1.0f)) {
        return *this;
    } else if (!MathUtil::FuzzyIsNull(len)) {
        double sqrtLen = std::sqrt(len);
        return DVector4D(double(double(v[0]) / sqrtLen), double(double(v[1]) / sqrtLen),
                         double(double(v[2]) / sqrtLen), double(double(v[3]) / sqrtLen));
    } else {
        return DVector4D();
    }
}

void DVector4D::normalize() {
    double len = double(v[0]) * double(v[0]) + double(v[1]) * double(v[1]) + double(v[2]) * double(v[2]) +
                 double(v[3]) * double(v[3]);
    if (MathUtil::FuzzyIsNull(len - 1.0f) || MathUtil::FuzzyIsNull(len)) return;

    len = std::sqrt(len);

    v[0] = double(double(v[0]) / len);
    v[1] = double(double(v[1]) / len);
    v[2] = double(double(v[2]) / len);
    v[3] = double(double(v[3]) / len);
}

DVector2D DVector4D::toVector2D() const { return DVector2D(v[0], v[1]); }

DVector2D DVector4D::toVector2DAffine() const {
    if (MathUtil::IsNull(v[3])) { return DVector2D(); }
    return DVector2D(v[0] / v[3], v[1] / v[3]);
}

DVector3D DVector4D::toVector3D() const { return DVector3D(v[0], v[1], v[2]); }

DVector3D DVector4D::toVector3DAffine() const {
    if (MathUtil::IsNull(v[3])) { return DVector3D(); }
    return DVector3D(v[0] / v[3], v[1] / v[3], v[2] / v[3]);
}
}  // namespace geometry
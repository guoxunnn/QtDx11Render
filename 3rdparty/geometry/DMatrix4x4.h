#ifndef _DMatrix4x4_H_
#define _DMatrix4x4_H_
#include <assert.h>

#include "geometry/DVector3D.h"
#include "geometry/DVector4D.h"
#include "geometry/GenericMatrix.h"
#include "geometry/MathUtil.h"
namespace geometry {
class DQuaternion;

class DMatrix4x4 {
public:
    inline DMatrix4x4() { setToIdentity(); }
    explicit DMatrix4x4(const Initialization&) : flagBits(General) {}
    explicit DMatrix4x4(const double* values);
    explicit DMatrix4x4(const double* values, int bits);
    inline DMatrix4x4(double m11, double m12, double m13, double m14, double m21, double m22, double m23,
                      double m24, double m31, double m32, double m33, double m34, double m41, double m42,
                      double m43, double m44) {
        m[0][0] = m11;
        m[0][1] = m21;
        m[0][2] = m31;
        m[0][3] = m41;
        m[1][0] = m12;
        m[1][1] = m22;
        m[1][2] = m32;
        m[1][3] = m42;
        m[2][0] = m13;
        m[2][1] = m23;
        m[2][2] = m33;
        m[2][3] = m43;
        m[3][0] = m14;
        m[3][1] = m24;
        m[3][2] = m34;
        m[3][3] = m44;
        flagBits = General;
    }

    template <int N, int M>
    explicit DMatrix4x4(const GenericMatrix<N, M, double>& matrix) {
        const double* values = matrix.constData();
        for (int matrixCol = 0; matrixCol < 4; ++matrixCol) {
            for (int matrixRow = 0; matrixRow < 4; ++matrixRow) {
                if (matrixCol < N && matrixRow < M)
                    m[matrixCol][matrixRow] = values[matrixCol * M + matrixRow];
                else if (matrixCol == matrixRow)
                    m[matrixCol][matrixRow] = 1.0f;
                else
                    m[matrixCol][matrixRow] = 0.0f;
            }
        }
        flagBits = General;
    }

    DMatrix4x4(const double* values, int cols, int rows);

    inline const double& operator()(int row, int column) const;
    inline double& operator()(int row, int column);

    inline bool isAffine() const;

    inline bool isIdentity() const;
    inline void setToIdentity();

    inline void fill(double value);

    double determinant() const;
    DMatrix4x4 inverted(bool* invertible = nullptr) const;
    DMatrix4x4 transposed() const;
    DMatrix3x3 normalMatrix() const;

    inline DMatrix4x4& operator+=(const DMatrix4x4& other);
    inline DMatrix4x4& operator-=(const DMatrix4x4& other);
    inline DMatrix4x4& operator*=(const DMatrix4x4& other);
    inline DMatrix4x4& operator*=(double factor);
    DMatrix4x4& operator/=(double divisor);
    inline bool operator==(const DMatrix4x4& other) const;
    inline bool operator!=(const DMatrix4x4& other) const;

    friend inline DMatrix4x4 operator+(const DMatrix4x4& m1, const DMatrix4x4& m2);
    friend inline DMatrix4x4 operator-(const DMatrix4x4& m1, const DMatrix4x4& m2);
    friend inline DMatrix4x4 operator*(const DMatrix4x4& m1, const DMatrix4x4& m2);

    friend inline DVector3D operator*(const DMatrix4x4& matrix, const DVector3D& vector);
    friend inline DVector3D operator*(const DVector3D& vector, const DMatrix4x4& matrix);
    friend inline DVector4D operator*(const DVector4D& vector, const DMatrix4x4& matrix);
    friend inline DVector4D operator*(const DMatrix4x4& matrix, const DVector4D& vector);

    friend inline DMatrix4x4 operator-(const DMatrix4x4& matrix);
    friend inline DMatrix4x4 operator*(double factor, const DMatrix4x4& matrix);
    friend inline DMatrix4x4 operator*(const DMatrix4x4& matrix, double factor);
    friend inline DMatrix4x4 operator/(const DMatrix4x4& matrix, double divisor);

    friend bool qFuzzyCompare(const DMatrix4x4& m1, const DMatrix4x4& m2);

    void scale(const DVector3D& vector);
    void scale(double x, double y);
    void scale(double x, double y, double z);
    void scale(double factor);

    void translate(const DVector3D& vector);
    void translate(double x, double y);
    void translate(double x, double y, double z);

    void rotate(double angle, const DVector3D& vector);
    void rotate(double angle, double x, double y, double z = 0.0f);
    void rotate(const DQuaternion& quaternion);

    void ortho(double left, double right, double bottom, double top, double nearPlane, double farPlane);
    void frustum(double left, double right, double bottom, double top, double nearPlane, double farPlane);
    void perspective(double verticalAngle, double aspectRatio, double nearPlane, double farPlane);

    void lookAt(const DVector3D& eye, const DVector3D& center, const DVector3D& up);

    void viewport(double left, double bottom, double width, double height, double nearPlane = 0.0f,
                  double farPlane = 1.0f);
    void flipCoordinates();
    void copyDataTo(double* values) const;

    inline DVector3D map(const DVector3D& point) const;

    template <int N, int M>
    GenericMatrix<N, M, double> toGenericMatrix() const {
        GenericMatrix<N, M, double> result;
        double* values = result.data();
        for (int matrixCol = 0; matrixCol < N; ++matrixCol) {
            for (int matrixRow = 0; matrixRow < M; ++matrixRow) {
                if (matrixCol < 4 && matrixRow < 4)
                    values[matrixCol * M + matrixRow] = m[matrixCol][matrixRow];
                else if (matrixCol == matrixRow)
                    values[matrixCol * M + matrixRow] = 1.0f;
                else
                    values[matrixCol * M + matrixRow] = 0.0f;
            }
        }
        return result;
    }

    inline double* data();
    inline const double* data() const { return *m; }
    inline const double* constData() const { return *m; }
    inline auto flag() -> int { return this->flagBits; }

    void optimize();

    void projectedRotate(double angle, double x, double y, double z);

    enum {
        Identity = 0x0000,     // Identity matrix
        Translation = 0x0001,  // Contains a translation
        Scale = 0x0002,        // Contains a scale
        Rotation2D = 0x0004,   // Contains a rotation about the Z axis
        Rotation = 0x0008,     // Contains an arbitrary rotation
        Perspective = 0x0010,  // Last row is different from (0, 0, 0, 1)
        General = 0x001f       // General matrix, unknown contents
    };

private:
    double m[4][4];  // Column-major order to match OpenGL.
    int flagBits;

    DMatrix4x4 orthonormalInverse() const;
};

inline const double& DMatrix4x4::operator()(int aRow, int aColumn) const {
    assert(aRow >= 0 && aRow < 4 && aColumn >= 0 && aColumn < 4);
    return m[aColumn][aRow];
}

inline double& DMatrix4x4::operator()(int aRow, int aColumn) {
    assert(aRow >= 0 && aRow < 4 && aColumn >= 0 && aColumn < 4);
    flagBits = General;
    return m[aColumn][aRow];
}

inline DMatrix4x4 operator/(const DMatrix4x4& matrix, double divisor) {
    DMatrix4x4 m(Initialization::Uninitialized);
    m.m[0][0] = matrix.m[0][0] / divisor;
    m.m[0][1] = matrix.m[0][1] / divisor;
    m.m[0][2] = matrix.m[0][2] / divisor;
    m.m[0][3] = matrix.m[0][3] / divisor;
    m.m[1][0] = matrix.m[1][0] / divisor;
    m.m[1][1] = matrix.m[1][1] / divisor;
    m.m[1][2] = matrix.m[1][2] / divisor;
    m.m[1][3] = matrix.m[1][3] / divisor;
    m.m[2][0] = matrix.m[2][0] / divisor;
    m.m[2][1] = matrix.m[2][1] / divisor;
    m.m[2][2] = matrix.m[2][2] / divisor;
    m.m[2][3] = matrix.m[2][3] / divisor;
    m.m[3][0] = matrix.m[3][0] / divisor;
    m.m[3][1] = matrix.m[3][1] / divisor;
    m.m[3][2] = matrix.m[3][2] / divisor;
    m.m[3][3] = matrix.m[3][3] / divisor;
    m.flagBits = DMatrix4x4::General;
    return m;
}

inline bool DMatrix4x4::isAffine() const {
    return m[0][3] == 0.0f && m[1][3] == 0.0f && m[2][3] == 0.0f && m[3][3] == 1.0f;
}

inline bool DMatrix4x4::isIdentity() const {
    if (flagBits == Identity) return true;
    if (m[0][0] != 1.0f || m[0][1] != 0.0f || m[0][2] != 0.0f) return false;
    if (m[0][3] != 0.0f || m[1][0] != 0.0f || m[1][1] != 1.0f) return false;
    if (m[1][2] != 0.0f || m[1][3] != 0.0f || m[2][0] != 0.0f) return false;
    if (m[2][1] != 0.0f || m[2][2] != 1.0f || m[2][3] != 0.0f) return false;
    if (m[3][0] != 0.0f || m[3][1] != 0.0f || m[3][2] != 0.0f) return false;
    return (m[3][3] == 1.0f);
}

inline void DMatrix4x4::setToIdentity() {
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
    flagBits = Identity;
}

inline void DMatrix4x4::fill(double value) {
    m[0][0] = value;
    m[0][1] = value;
    m[0][2] = value;
    m[0][3] = value;
    m[1][0] = value;
    m[1][1] = value;
    m[1][2] = value;
    m[1][3] = value;
    m[2][0] = value;
    m[2][1] = value;
    m[2][2] = value;
    m[2][3] = value;
    m[3][0] = value;
    m[3][1] = value;
    m[3][2] = value;
    m[3][3] = value;
    flagBits = General;
}

inline DMatrix4x4& DMatrix4x4::operator+=(const DMatrix4x4& other) {
    m[0][0] += other.m[0][0];
    m[0][1] += other.m[0][1];
    m[0][2] += other.m[0][2];
    m[0][3] += other.m[0][3];
    m[1][0] += other.m[1][0];
    m[1][1] += other.m[1][1];
    m[1][2] += other.m[1][2];
    m[1][3] += other.m[1][3];
    m[2][0] += other.m[2][0];
    m[2][1] += other.m[2][1];
    m[2][2] += other.m[2][2];
    m[2][3] += other.m[2][3];
    m[3][0] += other.m[3][0];
    m[3][1] += other.m[3][1];
    m[3][2] += other.m[3][2];
    m[3][3] += other.m[3][3];
    flagBits = General;
    return *this;
}

inline DMatrix4x4& DMatrix4x4::operator-=(const DMatrix4x4& other) {
    m[0][0] -= other.m[0][0];
    m[0][1] -= other.m[0][1];
    m[0][2] -= other.m[0][2];
    m[0][3] -= other.m[0][3];
    m[1][0] -= other.m[1][0];
    m[1][1] -= other.m[1][1];
    m[1][2] -= other.m[1][2];
    m[1][3] -= other.m[1][3];
    m[2][0] -= other.m[2][0];
    m[2][1] -= other.m[2][1];
    m[2][2] -= other.m[2][2];
    m[2][3] -= other.m[2][3];
    m[3][0] -= other.m[3][0];
    m[3][1] -= other.m[3][1];
    m[3][2] -= other.m[3][2];
    m[3][3] -= other.m[3][3];
    flagBits = General;
    return *this;
}

inline DMatrix4x4& DMatrix4x4::operator*=(const DMatrix4x4& o) {
    const DMatrix4x4 other = o;  // prevent aliasing when &o == this ### Qt 6: take o by value
    flagBits |= other.flagBits;

    if (flagBits < Rotation2D) {
        m[3][0] += m[0][0] * other.m[3][0];
        m[3][1] += m[1][1] * other.m[3][1];
        m[3][2] += m[2][2] * other.m[3][2];

        m[0][0] *= other.m[0][0];
        m[1][1] *= other.m[1][1];
        m[2][2] *= other.m[2][2];
        return *this;
    }

    double m0, m1, m2;
    m0 =
        m[0][0] * other.m[0][0] + m[1][0] * other.m[0][1] + m[2][0] * other.m[0][2] + m[3][0] * other.m[0][3];
    m1 =
        m[0][0] * other.m[1][0] + m[1][0] * other.m[1][1] + m[2][0] * other.m[1][2] + m[3][0] * other.m[1][3];
    m2 =
        m[0][0] * other.m[2][0] + m[1][0] * other.m[2][1] + m[2][0] * other.m[2][2] + m[3][0] * other.m[2][3];
    m[3][0] =
        m[0][0] * other.m[3][0] + m[1][0] * other.m[3][1] + m[2][0] * other.m[3][2] + m[3][0] * other.m[3][3];
    m[0][0] = m0;
    m[1][0] = m1;
    m[2][0] = m2;

    m0 =
        m[0][1] * other.m[0][0] + m[1][1] * other.m[0][1] + m[2][1] * other.m[0][2] + m[3][1] * other.m[0][3];
    m1 =
        m[0][1] * other.m[1][0] + m[1][1] * other.m[1][1] + m[2][1] * other.m[1][2] + m[3][1] * other.m[1][3];
    m2 =
        m[0][1] * other.m[2][0] + m[1][1] * other.m[2][1] + m[2][1] * other.m[2][2] + m[3][1] * other.m[2][3];
    m[3][1] =
        m[0][1] * other.m[3][0] + m[1][1] * other.m[3][1] + m[2][1] * other.m[3][2] + m[3][1] * other.m[3][3];
    m[0][1] = m0;
    m[1][1] = m1;
    m[2][1] = m2;

    m0 =
        m[0][2] * other.m[0][0] + m[1][2] * other.m[0][1] + m[2][2] * other.m[0][2] + m[3][2] * other.m[0][3];
    m1 =
        m[0][2] * other.m[1][0] + m[1][2] * other.m[1][1] + m[2][2] * other.m[1][2] + m[3][2] * other.m[1][3];
    m2 =
        m[0][2] * other.m[2][0] + m[1][2] * other.m[2][1] + m[2][2] * other.m[2][2] + m[3][2] * other.m[2][3];
    m[3][2] =
        m[0][2] * other.m[3][0] + m[1][2] * other.m[3][1] + m[2][2] * other.m[3][2] + m[3][2] * other.m[3][3];
    m[0][2] = m0;
    m[1][2] = m1;
    m[2][2] = m2;

    m0 =
        m[0][3] * other.m[0][0] + m[1][3] * other.m[0][1] + m[2][3] * other.m[0][2] + m[3][3] * other.m[0][3];
    m1 =
        m[0][3] * other.m[1][0] + m[1][3] * other.m[1][1] + m[2][3] * other.m[1][2] + m[3][3] * other.m[1][3];
    m2 =
        m[0][3] * other.m[2][0] + m[1][3] * other.m[2][1] + m[2][3] * other.m[2][2] + m[3][3] * other.m[2][3];
    m[3][3] =
        m[0][3] * other.m[3][0] + m[1][3] * other.m[3][1] + m[2][3] * other.m[3][2] + m[3][3] * other.m[3][3];
    m[0][3] = m0;
    m[1][3] = m1;
    m[2][3] = m2;
    return *this;
}

inline DMatrix4x4& DMatrix4x4::operator*=(double factor) {
    m[0][0] *= factor;
    m[0][1] *= factor;
    m[0][2] *= factor;
    m[0][3] *= factor;
    m[1][0] *= factor;
    m[1][1] *= factor;
    m[1][2] *= factor;
    m[1][3] *= factor;
    m[2][0] *= factor;
    m[2][1] *= factor;
    m[2][2] *= factor;
    m[2][3] *= factor;
    m[3][0] *= factor;
    m[3][1] *= factor;
    m[3][2] *= factor;
    m[3][3] *= factor;
    flagBits = General;
    return *this;
}

inline bool DMatrix4x4::operator==(const DMatrix4x4& other) const {
    return m[0][0] == other.m[0][0] && m[0][1] == other.m[0][1] && m[0][2] == other.m[0][2] &&
           m[0][3] == other.m[0][3] && m[1][0] == other.m[1][0] && m[1][1] == other.m[1][1] &&
           m[1][2] == other.m[1][2] && m[1][3] == other.m[1][3] && m[2][0] == other.m[2][0] &&
           m[2][1] == other.m[2][1] && m[2][2] == other.m[2][2] && m[2][3] == other.m[2][3] &&
           m[3][0] == other.m[3][0] && m[3][1] == other.m[3][1] && m[3][2] == other.m[3][2] &&
           m[3][3] == other.m[3][3];
}

inline bool DMatrix4x4::operator!=(const DMatrix4x4& other) const {
    return m[0][0] != other.m[0][0] || m[0][1] != other.m[0][1] || m[0][2] != other.m[0][2] ||
           m[0][3] != other.m[0][3] || m[1][0] != other.m[1][0] || m[1][1] != other.m[1][1] ||
           m[1][2] != other.m[1][2] || m[1][3] != other.m[1][3] || m[2][0] != other.m[2][0] ||
           m[2][1] != other.m[2][1] || m[2][2] != other.m[2][2] || m[2][3] != other.m[2][3] ||
           m[3][0] != other.m[3][0] || m[3][1] != other.m[3][1] || m[3][2] != other.m[3][2] ||
           m[3][3] != other.m[3][3];
}

inline DMatrix4x4 operator+(const DMatrix4x4& m1, const DMatrix4x4& m2) {
    DMatrix4x4 m(Initialization::Uninitialized);
    m.m[0][0] = m1.m[0][0] + m2.m[0][0];
    m.m[0][1] = m1.m[0][1] + m2.m[0][1];
    m.m[0][2] = m1.m[0][2] + m2.m[0][2];
    m.m[0][3] = m1.m[0][3] + m2.m[0][3];
    m.m[1][0] = m1.m[1][0] + m2.m[1][0];
    m.m[1][1] = m1.m[1][1] + m2.m[1][1];
    m.m[1][2] = m1.m[1][2] + m2.m[1][2];
    m.m[1][3] = m1.m[1][3] + m2.m[1][3];
    m.m[2][0] = m1.m[2][0] + m2.m[2][0];
    m.m[2][1] = m1.m[2][1] + m2.m[2][1];
    m.m[2][2] = m1.m[2][2] + m2.m[2][2];
    m.m[2][3] = m1.m[2][3] + m2.m[2][3];
    m.m[3][0] = m1.m[3][0] + m2.m[3][0];
    m.m[3][1] = m1.m[3][1] + m2.m[3][1];
    m.m[3][2] = m1.m[3][2] + m2.m[3][2];
    m.m[3][3] = m1.m[3][3] + m2.m[3][3];
    return m;
}

inline DMatrix4x4 operator-(const DMatrix4x4& m1, const DMatrix4x4& m2) {
    DMatrix4x4 m(Initialization::Uninitialized);
    m.m[0][0] = m1.m[0][0] - m2.m[0][0];
    m.m[0][1] = m1.m[0][1] - m2.m[0][1];
    m.m[0][2] = m1.m[0][2] - m2.m[0][2];
    m.m[0][3] = m1.m[0][3] - m2.m[0][3];
    m.m[1][0] = m1.m[1][0] - m2.m[1][0];
    m.m[1][1] = m1.m[1][1] - m2.m[1][1];
    m.m[1][2] = m1.m[1][2] - m2.m[1][2];
    m.m[1][3] = m1.m[1][3] - m2.m[1][3];
    m.m[2][0] = m1.m[2][0] - m2.m[2][0];
    m.m[2][1] = m1.m[2][1] - m2.m[2][1];
    m.m[2][2] = m1.m[2][2] - m2.m[2][2];
    m.m[2][3] = m1.m[2][3] - m2.m[2][3];
    m.m[3][0] = m1.m[3][0] - m2.m[3][0];
    m.m[3][1] = m1.m[3][1] - m2.m[3][1];
    m.m[3][2] = m1.m[3][2] - m2.m[3][2];
    m.m[3][3] = m1.m[3][3] - m2.m[3][3];
    return m;
}

inline DMatrix4x4 operator*(const DMatrix4x4& m1, const DMatrix4x4& m2) {
    int flagBits = m1.flagBits | m2.flagBits;
    if (flagBits < DMatrix4x4::Rotation2D) {
        DMatrix4x4 m = m1;
        m.m[3][0] += m.m[0][0] * m2.m[3][0];
        m.m[3][1] += m.m[1][1] * m2.m[3][1];
        m.m[3][2] += m.m[2][2] * m2.m[3][2];

        m.m[0][0] *= m2.m[0][0];
        m.m[1][1] *= m2.m[1][1];
        m.m[2][2] *= m2.m[2][2];
        m.flagBits = flagBits;
        return m;
    }

    DMatrix4x4 m(Initialization::Uninitialized);
    m.m[0][0] =
        m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2] + m1.m[3][0] * m2.m[0][3];
    m.m[0][1] =
        m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2] + m1.m[3][1] * m2.m[0][3];
    m.m[0][2] =
        m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2] + m1.m[3][2] * m2.m[0][3];
    m.m[0][3] =
        m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] + m1.m[2][3] * m2.m[0][2] + m1.m[3][3] * m2.m[0][3];

    m.m[1][0] =
        m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2] + m1.m[3][0] * m2.m[1][3];
    m.m[1][1] =
        m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2] + m1.m[3][1] * m2.m[1][3];
    m.m[1][2] =
        m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2] + m1.m[3][2] * m2.m[1][3];
    m.m[1][3] =
        m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] + m1.m[2][3] * m2.m[1][2] + m1.m[3][3] * m2.m[1][3];

    m.m[2][0] =
        m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[2][3];
    m.m[2][1] =
        m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[2][3];
    m.m[2][2] =
        m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[2][3];
    m.m[2][3] =
        m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] + m1.m[2][3] * m2.m[2][2] + m1.m[3][3] * m2.m[2][3];

    m.m[3][0] =
        m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] + m1.m[2][0] * m2.m[3][2] + m1.m[3][0] * m2.m[3][3];
    m.m[3][1] =
        m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] + m1.m[2][1] * m2.m[3][2] + m1.m[3][1] * m2.m[3][3];
    m.m[3][2] =
        m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] + m1.m[2][2] * m2.m[3][2] + m1.m[3][2] * m2.m[3][3];
    m.m[3][3] =
        m1.m[0][3] * m2.m[3][0] + m1.m[1][3] * m2.m[3][1] + m1.m[2][3] * m2.m[3][2] + m1.m[3][3] * m2.m[3][3];
    m.flagBits = flagBits;
    return m;
}

inline DVector3D operator*(const DVector3D& vector, const DMatrix4x4& matrix) {
    double x, y, z, w;
    x = vector.x() * matrix.m[0][0] + vector.y() * matrix.m[0][1] + vector.z() * matrix.m[0][2] +
        matrix.m[0][3];
    y = vector.x() * matrix.m[1][0] + vector.y() * matrix.m[1][1] + vector.z() * matrix.m[1][2] +
        matrix.m[1][3];
    z = vector.x() * matrix.m[2][0] + vector.y() * matrix.m[2][1] + vector.z() * matrix.m[2][2] +
        matrix.m[2][3];
    w = vector.x() * matrix.m[3][0] + vector.y() * matrix.m[3][1] + vector.z() * matrix.m[3][2] +
        matrix.m[3][3];
    if (w == 1.0f)
        return DVector3D(x, y, z);
    else
        return DVector3D(x / w, y / w, z / w);
}

inline DVector3D operator*(const DMatrix4x4& matrix, const DVector3D& vector) { return matrix.map(vector); }

inline DMatrix4x4 operator-(const DMatrix4x4& matrix) {
    DMatrix4x4 m(Initialization::Uninitialized);
    m.m[0][0] = -matrix.m[0][0];
    m.m[0][1] = -matrix.m[0][1];
    m.m[0][2] = -matrix.m[0][2];
    m.m[0][3] = -matrix.m[0][3];
    m.m[1][0] = -matrix.m[1][0];
    m.m[1][1] = -matrix.m[1][1];
    m.m[1][2] = -matrix.m[1][2];
    m.m[1][3] = -matrix.m[1][3];
    m.m[2][0] = -matrix.m[2][0];
    m.m[2][1] = -matrix.m[2][1];
    m.m[2][2] = -matrix.m[2][2];
    m.m[2][3] = -matrix.m[2][3];
    m.m[3][0] = -matrix.m[3][0];
    m.m[3][1] = -matrix.m[3][1];
    m.m[3][2] = -matrix.m[3][2];
    m.m[3][3] = -matrix.m[3][3];
    return m;
}

inline DMatrix4x4 operator*(double factor, const DMatrix4x4& matrix) {
    DMatrix4x4 m(Initialization::Uninitialized);
    m.m[0][0] = matrix.m[0][0] * factor;
    m.m[0][1] = matrix.m[0][1] * factor;
    m.m[0][2] = matrix.m[0][2] * factor;
    m.m[0][3] = matrix.m[0][3] * factor;
    m.m[1][0] = matrix.m[1][0] * factor;
    m.m[1][1] = matrix.m[1][1] * factor;
    m.m[1][2] = matrix.m[1][2] * factor;
    m.m[1][3] = matrix.m[1][3] * factor;
    m.m[2][0] = matrix.m[2][0] * factor;
    m.m[2][1] = matrix.m[2][1] * factor;
    m.m[2][2] = matrix.m[2][2] * factor;
    m.m[2][3] = matrix.m[2][3] * factor;
    m.m[3][0] = matrix.m[3][0] * factor;
    m.m[3][1] = matrix.m[3][1] * factor;
    m.m[3][2] = matrix.m[3][2] * factor;
    m.m[3][3] = matrix.m[3][3] * factor;
    return m;
}

inline DMatrix4x4 operator*(const DMatrix4x4& matrix, double factor) {
    DMatrix4x4 m(Initialization::Uninitialized);
    m.m[0][0] = matrix.m[0][0] * factor;
    m.m[0][1] = matrix.m[0][1] * factor;
    m.m[0][2] = matrix.m[0][2] * factor;
    m.m[0][3] = matrix.m[0][3] * factor;
    m.m[1][0] = matrix.m[1][0] * factor;
    m.m[1][1] = matrix.m[1][1] * factor;
    m.m[1][2] = matrix.m[1][2] * factor;
    m.m[1][3] = matrix.m[1][3] * factor;
    m.m[2][0] = matrix.m[2][0] * factor;
    m.m[2][1] = matrix.m[2][1] * factor;
    m.m[2][2] = matrix.m[2][2] * factor;
    m.m[2][3] = matrix.m[2][3] * factor;
    m.m[3][0] = matrix.m[3][0] * factor;
    m.m[3][1] = matrix.m[3][1] * factor;
    m.m[3][2] = matrix.m[3][2] * factor;
    m.m[3][3] = matrix.m[3][3] * factor;
    return m;
}

inline DVector3D DMatrix4x4::map(const DVector3D& point) const {
    double x, y, z, w;
    if (flagBits == DMatrix4x4::Identity) {
        return point;
    } else if (flagBits < DMatrix4x4::Rotation2D) {
        // Translation | Scale
        return DVector3D(point.x() * m[0][0] + m[3][0], point.y() * m[1][1] + m[3][1],
                         point.z() * m[2][2] + m[3][2]);
    } else if (flagBits < DMatrix4x4::Rotation) {
        // Translation | Scale | Rotation2D
        return DVector3D(point.x() * m[0][0] + point.y() * m[1][0] + m[3][0],
                         point.x() * m[0][1] + point.y() * m[1][1] + m[3][1], point.z() * m[2][2] + m[3][2]);
    } else {
        x = point.x() * m[0][0] + point.y() * m[1][0] + point.z() * m[2][0] + m[3][0];
        y = point.x() * m[0][1] + point.y() * m[1][1] + point.z() * m[2][1] + m[3][1];
        z = point.x() * m[0][2] + point.y() * m[1][2] + point.z() * m[2][2] + m[3][2];
        w = point.x() * m[0][3] + point.y() * m[1][3] + point.z() * m[2][3] + m[3][3];
        if (w == 1.0f)
            return DVector3D(x, y, z);
        else
            return DVector3D(x / w, y / w, z / w);
    }
}

inline double* DMatrix4x4::data() {
    // We have to assume that the caller will modify the matrix elements,
    // so we flip it over to "General" mode.
    flagBits = General;
    return *m;
}

inline DVector4D operator*(const DVector4D& vector, const DMatrix4x4& matrix) {
    double x, y, z, w;
    x = vector.x() * matrix.m[0][0] + vector.y() * matrix.m[0][1] + vector.z() * matrix.m[0][2] +
        vector.w() * matrix.m[0][3];
    y = vector.x() * matrix.m[1][0] + vector.y() * matrix.m[1][1] + vector.z() * matrix.m[1][2] +
        vector.w() * matrix.m[1][3];
    z = vector.x() * matrix.m[2][0] + vector.y() * matrix.m[2][1] + vector.z() * matrix.m[2][2] +
        vector.w() * matrix.m[2][3];
    w = vector.x() * matrix.m[3][0] + vector.y() * matrix.m[3][1] + vector.z() * matrix.m[3][2] +
        vector.w() * matrix.m[3][3];
    return DVector4D(x, y, z, w);
}

inline DVector4D operator*(const DMatrix4x4& matrix, const DVector4D& vector) {
    double x, y, z, w;
    x = vector.x() * matrix.m[0][0] + vector.y() * matrix.m[1][0] + vector.z() * matrix.m[2][0] +
        vector.w() * matrix.m[3][0];
    y = vector.x() * matrix.m[0][1] + vector.y() * matrix.m[1][1] + vector.z() * matrix.m[2][1] +
        vector.w() * matrix.m[3][1];
    z = vector.x() * matrix.m[0][2] + vector.y() * matrix.m[1][2] + vector.z() * matrix.m[2][2] +
        vector.w() * matrix.m[3][2];
    w = vector.x() * matrix.m[0][3] + vector.y() * matrix.m[1][3] + vector.z() * matrix.m[2][3] +
        vector.w() * matrix.m[3][3];
    return DVector4D(x, y, z, w);
}
}  // namespace geometry
#endif  // _DMatrix4x4_H_
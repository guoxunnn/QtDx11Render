#ifndef _FMatrix4x4_H_
#define _FMatrix4x4_H_

#include "geometry/FVector3D.h"
#include "geometry/FVector4D.h"
#include "geometry/GenericMatrix.h"
#include "geometry/MathUtil.h"
namespace geometry {
class FQuaternion;

class FMatrix4x4 {
public:
    inline FMatrix4x4() { setToIdentity(); }
    explicit FMatrix4x4(const Initialization&) : flagBits(General) {}
    explicit FMatrix4x4(const float* values);
    explicit FMatrix4x4(const float* values, int bits);
    inline FMatrix4x4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
                      float m31, float m32, float m33, float m34, float m41, float m42, float m43,
                      float m44) {
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
    explicit FMatrix4x4(const GenericMatrix<N, M, float>& matrix) {
        const float* values = matrix.constData();
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

    FMatrix4x4(const float* values, int cols, int rows);

    inline const float& operator()(int row, int column) const;
    inline float& operator()(int row, int column);

    inline bool isAffine() const;

    inline bool isIdentity() const;
    inline void setToIdentity();

    inline void fill(float value);

    double determinant() const;
    FMatrix4x4 inverted(bool* invertible = nullptr) const;
    FMatrix4x4 transposed() const;
    FMatrix3x3 normalMatrix() const;

    inline FMatrix4x4& operator+=(const FMatrix4x4& other);
    inline FMatrix4x4& operator-=(const FMatrix4x4& other);
    inline FMatrix4x4& operator*=(const FMatrix4x4& other);
    inline FMatrix4x4& operator*=(float factor);
    FMatrix4x4& operator/=(float divisor);
    inline bool operator==(const FMatrix4x4& other) const;
    inline bool operator!=(const FMatrix4x4& other) const;

    friend inline FMatrix4x4 operator+(const FMatrix4x4& m1, const FMatrix4x4& m2);
    friend inline FMatrix4x4 operator-(const FMatrix4x4& m1, const FMatrix4x4& m2);
    friend inline FMatrix4x4 operator*(const FMatrix4x4& m1, const FMatrix4x4& m2);

    friend inline FVector3D operator*(const FMatrix4x4& matrix, const FVector3D& vector);
    friend inline FVector3D operator*(const FVector3D& vector, const FMatrix4x4& matrix);
    friend inline FVector4D operator*(const FVector4D& vector, const FMatrix4x4& matrix);
    friend inline FVector4D operator*(const FMatrix4x4& matrix, const FVector4D& vector);

    friend inline FMatrix4x4 operator-(const FMatrix4x4& matrix);
    friend inline FMatrix4x4 operator*(float factor, const FMatrix4x4& matrix);
    friend inline FMatrix4x4 operator*(const FMatrix4x4& matrix, float factor);
    friend inline FMatrix4x4 operator/(const FMatrix4x4& matrix, float divisor);

    friend bool qFuzzyCompare(const FMatrix4x4& m1, const FMatrix4x4& m2);

    void scale(const FVector3D& vector);
    void scale(float x, float y);
    void scale(float x, float y, float z);
    void scale(float factor);

    void translate(const FVector3D& vector);
    void translate(float x, float y);
    void translate(float x, float y, float z);

    void rotate(float angle, const FVector3D& vector);
    void rotate(float angle, float x, float y, float z = 0.0f);
    //void rotate(const FQuaternion& quaternion);

    void ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    void frustum(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    void perspective(float verticalAngle, float aspectRatio, float nearPlane, float farPlane);

    void lookAt(const FVector3D& eye, const FVector3D& center, const FVector3D& up);

    void viewport(float left, float bottom, float width, float height, float nearPlane = 0.0f,
                  float farPlane = 1.0f);
    void flipCoordinates();

    void copyDataTo(float* values) const;

    inline FVector3D map(const FVector3D& point) const;

    template <int N, int M>
    GenericMatrix<N, M, float> toGenericMatrix() const {
        GenericMatrix<N, M, float> result;
        float* values = result.data();
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

    inline float* data();
    inline const float* data() const { return *m; }
    inline const float* constData() const { return *m; }
    inline auto flag() -> int { return this->flagBits; }

    void optimize();

    void projectedRotate(float angle, float x, float y, float z);

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
    float m[4][4];  // Column-major order to match OpenGL.
    int flagBits;

    FMatrix4x4 orthonormalInverse() const;
};

inline const float& FMatrix4x4::operator()(int aRow, int aColumn) const {
    assert(aRow >= 0 && aRow < 4 && aColumn >= 0 && aColumn < 4);
    return m[aColumn][aRow];
}

inline float& FMatrix4x4::operator()(int aRow, int aColumn) {
    assert(aRow >= 0 && aRow < 4 && aColumn >= 0 && aColumn < 4);
    flagBits = General;
    return m[aColumn][aRow];
}

inline FMatrix4x4 operator/(const FMatrix4x4& matrix, float divisor) {
    FMatrix4x4 m(Initialization::Uninitialized);
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
    m.flagBits = FMatrix4x4::General;
    return m;
}

inline bool FMatrix4x4::isAffine() const {
    return m[0][3] == 0.0f && m[1][3] == 0.0f && m[2][3] == 0.0f && m[3][3] == 1.0f;
}

inline bool FMatrix4x4::isIdentity() const {
    if (flagBits == Identity) return true;
    if (m[0][0] != 1.0f || m[0][1] != 0.0f || m[0][2] != 0.0f) return false;
    if (m[0][3] != 0.0f || m[1][0] != 0.0f || m[1][1] != 1.0f) return false;
    if (m[1][2] != 0.0f || m[1][3] != 0.0f || m[2][0] != 0.0f) return false;
    if (m[2][1] != 0.0f || m[2][2] != 1.0f || m[2][3] != 0.0f) return false;
    if (m[3][0] != 0.0f || m[3][1] != 0.0f || m[3][2] != 0.0f) return false;
    return (m[3][3] == 1.0f);
}

inline void FMatrix4x4::setToIdentity() {
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

inline void FMatrix4x4::fill(float value) {
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

inline FMatrix4x4& FMatrix4x4::operator+=(const FMatrix4x4& other) {
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

inline FMatrix4x4& FMatrix4x4::operator-=(const FMatrix4x4& other) {
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

inline FMatrix4x4& FMatrix4x4::operator*=(const FMatrix4x4& o) {
    const FMatrix4x4 other = o;  // prevent aliasing when &o == this ### Qt 6: take o by value
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

    float m0, m1, m2;
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

inline FMatrix4x4& FMatrix4x4::operator*=(float factor) {
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

inline bool FMatrix4x4::operator==(const FMatrix4x4& other) const {
    return m[0][0] == other.m[0][0] && m[0][1] == other.m[0][1] && m[0][2] == other.m[0][2] &&
           m[0][3] == other.m[0][3] && m[1][0] == other.m[1][0] && m[1][1] == other.m[1][1] &&
           m[1][2] == other.m[1][2] && m[1][3] == other.m[1][3] && m[2][0] == other.m[2][0] &&
           m[2][1] == other.m[2][1] && m[2][2] == other.m[2][2] && m[2][3] == other.m[2][3] &&
           m[3][0] == other.m[3][0] && m[3][1] == other.m[3][1] && m[3][2] == other.m[3][2] &&
           m[3][3] == other.m[3][3];
}

inline bool FMatrix4x4::operator!=(const FMatrix4x4& other) const {
    return m[0][0] != other.m[0][0] || m[0][1] != other.m[0][1] || m[0][2] != other.m[0][2] ||
           m[0][3] != other.m[0][3] || m[1][0] != other.m[1][0] || m[1][1] != other.m[1][1] ||
           m[1][2] != other.m[1][2] || m[1][3] != other.m[1][3] || m[2][0] != other.m[2][0] ||
           m[2][1] != other.m[2][1] || m[2][2] != other.m[2][2] || m[2][3] != other.m[2][3] ||
           m[3][0] != other.m[3][0] || m[3][1] != other.m[3][1] || m[3][2] != other.m[3][2] ||
           m[3][3] != other.m[3][3];
}

inline FMatrix4x4 operator+(const FMatrix4x4& m1, const FMatrix4x4& m2) {
    FMatrix4x4 m(Initialization::Uninitialized);
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

inline FMatrix4x4 operator-(const FMatrix4x4& m1, const FMatrix4x4& m2) {
    FMatrix4x4 m(Initialization::Uninitialized);
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

inline FMatrix4x4 operator*(const FMatrix4x4& m1, const FMatrix4x4& m2) {
    int flagBits = m1.flagBits | m2.flagBits;
    if (flagBits < FMatrix4x4::Rotation2D) {
        FMatrix4x4 m = m1;
        m.m[3][0] += m.m[0][0] * m2.m[3][0];
        m.m[3][1] += m.m[1][1] * m2.m[3][1];
        m.m[3][2] += m.m[2][2] * m2.m[3][2];

        m.m[0][0] *= m2.m[0][0];
        m.m[1][1] *= m2.m[1][1];
        m.m[2][2] *= m2.m[2][2];
        m.flagBits = flagBits;
        return m;
    }

    FMatrix4x4 m(Initialization::Uninitialized);
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

inline FVector3D operator*(const FVector3D& vector, const FMatrix4x4& matrix) {
    float x, y, z, w;
    x = vector.x() * matrix.m[0][0] + vector.y() * matrix.m[0][1] + vector.z() * matrix.m[0][2] +
        matrix.m[0][3];
    y = vector.x() * matrix.m[1][0] + vector.y() * matrix.m[1][1] + vector.z() * matrix.m[1][2] +
        matrix.m[1][3];
    z = vector.x() * matrix.m[2][0] + vector.y() * matrix.m[2][1] + vector.z() * matrix.m[2][2] +
        matrix.m[2][3];
    w = vector.x() * matrix.m[3][0] + vector.y() * matrix.m[3][1] + vector.z() * matrix.m[3][2] +
        matrix.m[3][3];
    if (w == 1.0f)
        return FVector3D(x, y, z);
    else
        return FVector3D(x / w, y / w, z / w);
}

inline FVector3D operator*(const FMatrix4x4& matrix, const FVector3D& vector) { return matrix.map(vector); }

inline FMatrix4x4 operator-(const FMatrix4x4& matrix) {
    FMatrix4x4 m(Initialization::Uninitialized);
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

inline FMatrix4x4 operator*(float factor, const FMatrix4x4& matrix) {
    FMatrix4x4 m(Initialization::Uninitialized);
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

inline FMatrix4x4 operator*(const FMatrix4x4& matrix, float factor) {
    FMatrix4x4 m(Initialization::Uninitialized);
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

inline FVector3D FMatrix4x4::map(const FVector3D& point) const {
    float x, y, z, w;
    if (flagBits == FMatrix4x4::Identity) {
        return point;
    } else if (flagBits < FMatrix4x4::Rotation2D) {
        // Translation | Scale
        return FVector3D(point.x() * m[0][0] + m[3][0], point.y() * m[1][1] + m[3][1],
                         point.z() * m[2][2] + m[3][2]);
    } else if (flagBits < FMatrix4x4::Rotation) {
        // Translation | Scale | Rotation2D
        return FVector3D(point.x() * m[0][0] + point.y() * m[1][0] + m[3][0],
                         point.x() * m[0][1] + point.y() * m[1][1] + m[3][1], point.z() * m[2][2] + m[3][2]);
    } else {
        x = point.x() * m[0][0] + point.y() * m[1][0] + point.z() * m[2][0] + m[3][0];
        y = point.x() * m[0][1] + point.y() * m[1][1] + point.z() * m[2][1] + m[3][1];
        z = point.x() * m[0][2] + point.y() * m[1][2] + point.z() * m[2][2] + m[3][2];
        w = point.x() * m[0][3] + point.y() * m[1][3] + point.z() * m[2][3] + m[3][3];
        if (w == 1.0f)
            return FVector3D(x, y, z);
        else
            return FVector3D(x / w, y / w, z / w);
    }
}

inline float* FMatrix4x4::data() {
    // We have to assume that the caller will modify the matrix elements,
    // so we flip it over to "General" mode.
    flagBits = General;
    return *m;
}

inline FVector4D operator*(const FVector4D& vector, const FMatrix4x4& matrix) {
    float x, y, z, w;
    x = vector.x() * matrix.m[0][0] + vector.y() * matrix.m[0][1] + vector.z() * matrix.m[0][2] +
        vector.w() * matrix.m[0][3];
    y = vector.x() * matrix.m[1][0] + vector.y() * matrix.m[1][1] + vector.z() * matrix.m[1][2] +
        vector.w() * matrix.m[1][3];
    z = vector.x() * matrix.m[2][0] + vector.y() * matrix.m[2][1] + vector.z() * matrix.m[2][2] +
        vector.w() * matrix.m[2][3];
    w = vector.x() * matrix.m[3][0] + vector.y() * matrix.m[3][1] + vector.z() * matrix.m[3][2] +
        vector.w() * matrix.m[3][3];
    return FVector4D(x, y, z, w);
}

inline FVector4D operator*(const FMatrix4x4& matrix, const FVector4D& vector) {
    float x, y, z, w;
    x = vector.x() * matrix.m[0][0] + vector.y() * matrix.m[1][0] + vector.z() * matrix.m[2][0] +
        vector.w() * matrix.m[3][0];
    y = vector.x() * matrix.m[0][1] + vector.y() * matrix.m[1][1] + vector.z() * matrix.m[2][1] +
        vector.w() * matrix.m[3][1];
    z = vector.x() * matrix.m[0][2] + vector.y() * matrix.m[1][2] + vector.z() * matrix.m[2][2] +
        vector.w() * matrix.m[3][2];
    w = vector.x() * matrix.m[0][3] + vector.y() * matrix.m[1][3] + vector.z() * matrix.m[2][3] +
        vector.w() * matrix.m[3][3];
    return FVector4D(x, y, z, w);
}
}  // namespace geometry
#endif  // _FMatrix4x4_H_

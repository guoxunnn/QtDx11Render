#include "DMatrix4x4.h"

#include "DQuaternion.h"
#include "MathUtil.h"
#include <cmath>
namespace geometry {
static const double inv_dist_to_plane = 1.0f / 1024.0f;

DMatrix4x4::DMatrix4x4(const double *values) : DMatrix4x4(values, (int)General) {}

DMatrix4x4::DMatrix4x4(const double *values, int bits) : flagBits(bits) {
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) { m[col][row] = values[row * 4 + col]; }
    }
}

static inline double matrixDet2(const double m[4][4], int col0, int col1, int row0, int row1) {
    return m[col0][row0] * m[col1][row1] - m[col0][row1] * m[col1][row0];
}

static inline double matrixDet3(const double m[4][4], int col0, int col1, int col2, int row0, int row1,
                                int row2) {
    return m[col0][row0] * matrixDet2(m, col1, col2, row1, row2) -
           m[col1][row0] * matrixDet2(m, col0, col2, row1, row2) +
           m[col2][row0] * matrixDet2(m, col0, col1, row1, row2);
}

static inline double matrixDet4(const double m[4][4]) {
    double det;
    det = m[0][0] * matrixDet3(m, 1, 2, 3, 1, 2, 3);
    det -= m[1][0] * matrixDet3(m, 0, 2, 3, 1, 2, 3);
    det += m[2][0] * matrixDet3(m, 0, 1, 3, 1, 2, 3);
    det -= m[3][0] * matrixDet3(m, 0, 1, 2, 1, 2, 3);
    return det;
}

static inline void copyToDoubles(const double m[4][4], double mm[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) { mm[i][j] = double(m[i][j]); }
    }
}

double DMatrix4x4::determinant() const {
    if ((flagBits & ~(Translation | Rotation2D | Rotation)) == Identity) { return 1.0; }

    double mm[4][4];
    copyToDoubles(m, mm);
    if (flagBits < Rotation2D) {
        return mm[0][0] * mm[1][1] * mm[2][2];  // Translation | Scale
    }
    if (flagBits < Perspective) { return matrixDet3(mm, 0, 1, 2, 0, 1, 2); }
    return matrixDet4(mm);
}

DMatrix4x4 DMatrix4x4::inverted(bool *invertible) const {
    // Handle some of the easy cases first.
    if (flagBits == Identity) {
        if (invertible) *invertible = true;
        return DMatrix4x4();
    } else if (flagBits == Translation) {
        DMatrix4x4 inv;
        inv.m[3][0] = -m[3][0];
        inv.m[3][1] = -m[3][1];
        inv.m[3][2] = -m[3][2];
        inv.flagBits = Translation;
        if (invertible) *invertible = true;
        return inv;
    } else if (flagBits < Rotation2D) {
        // Translation | Scale
        if (m[0][0] == 0 || m[1][1] == 0 || m[2][2] == 0) {
            if (invertible) *invertible = false;
            return DMatrix4x4();
        }
        DMatrix4x4 inv;
        inv.m[0][0] = 1.0f / m[0][0];
        inv.m[1][1] = 1.0f / m[1][1];
        inv.m[2][2] = 1.0f / m[2][2];
        inv.m[3][0] = -m[3][0] * inv.m[0][0];
        inv.m[3][1] = -m[3][1] * inv.m[1][1];
        inv.m[3][2] = -m[3][2] * inv.m[2][2];
        inv.flagBits = flagBits;

        if (invertible) *invertible = true;
        return inv;
    } else if ((flagBits & ~(Translation | Rotation2D | Rotation)) == Identity) {
        if (invertible) *invertible = true;
        return orthonormalInverse();
    } else if (flagBits < Perspective) {
        DMatrix4x4 inv(Initialization::Uninitialized);

        double mm[4][4];
        copyToDoubles(m, mm);

        double det = matrixDet3(mm, 0, 1, 2, 0, 1, 2);
        if (det == 0.0f) {
            if (invertible) *invertible = false;
            return DMatrix4x4();
        }
        det = 1.0f / det;

        inv.m[0][0] = matrixDet2(mm, 1, 2, 1, 2) * det;
        inv.m[0][1] = -matrixDet2(mm, 0, 2, 1, 2) * det;
        inv.m[0][2] = matrixDet2(mm, 0, 1, 1, 2) * det;
        inv.m[0][3] = 0;
        inv.m[1][0] = -matrixDet2(mm, 1, 2, 0, 2) * det;
        inv.m[1][1] = matrixDet2(mm, 0, 2, 0, 2) * det;
        inv.m[1][2] = -matrixDet2(mm, 0, 1, 0, 2) * det;
        inv.m[1][3] = 0;
        inv.m[2][0] = matrixDet2(mm, 1, 2, 0, 1) * det;
        inv.m[2][1] = -matrixDet2(mm, 0, 2, 0, 1) * det;
        inv.m[2][2] = matrixDet2(mm, 0, 1, 0, 1) * det;
        inv.m[2][3] = 0;
        inv.m[3][0] = -inv.m[0][0] * m[3][0] - inv.m[1][0] * m[3][1] - inv.m[2][0] * m[3][2];
        inv.m[3][1] = -inv.m[0][1] * m[3][0] - inv.m[1][1] * m[3][1] - inv.m[2][1] * m[3][2];
        inv.m[3][2] = -inv.m[0][2] * m[3][0] - inv.m[1][2] * m[3][1] - inv.m[2][2] * m[3][2];
        inv.m[3][3] = 1;
        inv.flagBits = flagBits;

        if (invertible) *invertible = true;
        return inv;
    }

    DMatrix4x4 inv(Initialization::Uninitialized);

    double mm[4][4];
    copyToDoubles(m, mm);

    double det = matrixDet4(mm);
    if (det == 0.0f) {
        if (invertible) *invertible = false;
        return DMatrix4x4();
    }
    det = 1.0f / det;

    inv.m[0][0] = matrixDet3(mm, 1, 2, 3, 1, 2, 3) * det;
    inv.m[0][1] = -matrixDet3(mm, 0, 2, 3, 1, 2, 3) * det;
    inv.m[0][2] = matrixDet3(mm, 0, 1, 3, 1, 2, 3) * det;
    inv.m[0][3] = -matrixDet3(mm, 0, 1, 2, 1, 2, 3) * det;
    inv.m[1][0] = -matrixDet3(mm, 1, 2, 3, 0, 2, 3) * det;
    inv.m[1][1] = matrixDet3(mm, 0, 2, 3, 0, 2, 3) * det;
    inv.m[1][2] = -matrixDet3(mm, 0, 1, 3, 0, 2, 3) * det;
    inv.m[1][3] = matrixDet3(mm, 0, 1, 2, 0, 2, 3) * det;
    inv.m[2][0] = matrixDet3(mm, 1, 2, 3, 0, 1, 3) * det;
    inv.m[2][1] = -matrixDet3(mm, 0, 2, 3, 0, 1, 3) * det;
    inv.m[2][2] = matrixDet3(mm, 0, 1, 3, 0, 1, 3) * det;
    inv.m[2][3] = -matrixDet3(mm, 0, 1, 2, 0, 1, 3) * det;
    inv.m[3][0] = -matrixDet3(mm, 1, 2, 3, 0, 1, 2) * det;
    inv.m[3][1] = matrixDet3(mm, 0, 2, 3, 0, 1, 2) * det;
    inv.m[3][2] = -matrixDet3(mm, 0, 1, 3, 0, 1, 2) * det;
    inv.m[3][3] = matrixDet3(mm, 0, 1, 2, 0, 1, 2) * det;
    inv.flagBits = flagBits;

    if (invertible) { *invertible = true; }
    return inv;
}

DMatrix4x4 DMatrix4x4::transposed() const {
    DMatrix4x4 result(Initialization::Uninitialized);
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) { result.m[col][row] = m[row][col]; }
    }
    // When a translation is transposed, it becomes a perspective transformation.
    result.flagBits = (flagBits & Translation ? General : flagBits);
    return result;
}

DMatrix3x3 DMatrix4x4::normalMatrix() const {
    DMatrix3x3 inv;

    // Handle the simple cases first.
    if (flagBits < Scale) {
        // Translation
        return inv;
    } else if (flagBits < Rotation2D) {
        // Translation | Scale
        if (m[0][0] == 0.0f || m[1][1] == 0.0f || m[2][2] == 0.0f) return inv;
        inv.data()[0] = 1.0f / m[0][0];
        inv.data()[4] = 1.0f / m[1][1];
        inv.data()[8] = 1.0f / m[2][2];
        return inv;
    } else if ((flagBits & ~(Translation | Rotation2D | Rotation)) == Identity) {
        double *invm = inv.data();
        invm[0 + 0 * 3] = m[0][0];
        invm[1 + 0 * 3] = m[0][1];
        invm[2 + 0 * 3] = m[0][2];
        invm[0 + 1 * 3] = m[1][0];
        invm[1 + 1 * 3] = m[1][1];
        invm[2 + 1 * 3] = m[1][2];
        invm[0 + 2 * 3] = m[2][0];
        invm[1 + 2 * 3] = m[2][1];
        invm[2 + 2 * 3] = m[2][2];
        return inv;
    }

    double mm[4][4];
    copyToDoubles(m, mm);
    double det = matrixDet3(mm, 0, 1, 2, 0, 1, 2);
    if (det == 0.0f) return inv;
    det = 1.0f / det;

    double *invm = inv.data();

    // Invert and transpose in a single step.
    invm[0 + 0 * 3] = (mm[1][1] * mm[2][2] - mm[2][1] * mm[1][2]) * det;
    invm[1 + 0 * 3] = -(mm[1][0] * mm[2][2] - mm[1][2] * mm[2][0]) * det;
    invm[2 + 0 * 3] = (mm[1][0] * mm[2][1] - mm[1][1] * mm[2][0]) * det;
    invm[0 + 1 * 3] = -(mm[0][1] * mm[2][2] - mm[2][1] * mm[0][2]) * det;
    invm[1 + 1 * 3] = (mm[0][0] * mm[2][2] - mm[0][2] * mm[2][0]) * det;
    invm[2 + 1 * 3] = -(mm[0][0] * mm[2][1] - mm[0][1] * mm[2][0]) * det;
    invm[0 + 2 * 3] = (mm[0][1] * mm[1][2] - mm[0][2] * mm[1][1]) * det;
    invm[1 + 2 * 3] = -(mm[0][0] * mm[1][2] - mm[0][2] * mm[1][0]) * det;
    invm[2 + 2 * 3] = (mm[0][0] * mm[1][1] - mm[1][0] * mm[0][1]) * det;

    return inv;
}

DMatrix4x4 &DMatrix4x4::operator/=(double divisor) {
    m[0][0] /= divisor;
    m[0][1] /= divisor;
    m[0][2] /= divisor;
    m[0][3] /= divisor;
    m[1][0] /= divisor;
    m[1][1] /= divisor;
    m[1][2] /= divisor;
    m[1][3] /= divisor;
    m[2][0] /= divisor;
    m[2][1] /= divisor;
    m[2][2] /= divisor;
    m[2][3] /= divisor;
    m[3][0] /= divisor;
    m[3][1] /= divisor;
    m[3][2] /= divisor;
    m[3][3] /= divisor;
    flagBits = General;
    return *this;
}

void DMatrix4x4::scale(const DVector3D &vector) {
    double vx = vector.x();
    double vy = vector.y();
    double vz = vector.z();
    if (flagBits < Scale) {
        m[0][0] = vx;
        m[1][1] = vy;
        m[2][2] = vz;
    } else if (flagBits < Rotation2D) {
        m[0][0] *= vx;
        m[1][1] *= vy;
        m[2][2] *= vz;
    } else if (flagBits < Rotation) {
        m[0][0] *= vx;
        m[0][1] *= vx;
        m[1][0] *= vy;
        m[1][1] *= vy;
        m[2][2] *= vz;
    } else {
        m[0][0] *= vx;
        m[0][1] *= vx;
        m[0][2] *= vx;
        m[0][3] *= vx;
        m[1][0] *= vy;
        m[1][1] *= vy;
        m[1][2] *= vy;
        m[1][3] *= vy;
        m[2][0] *= vz;
        m[2][1] *= vz;
        m[2][2] *= vz;
        m[2][3] *= vz;
    }
    flagBits |= Scale;
}

void DMatrix4x4::scale(double x, double y) {
    if (flagBits < Scale) {
        m[0][0] = x;
        m[1][1] = y;
    } else if (flagBits < Rotation2D) {
        m[0][0] *= x;
        m[1][1] *= y;
    } else if (flagBits < Rotation) {
        m[0][0] *= x;
        m[0][1] *= x;
        m[1][0] *= y;
        m[1][1] *= y;
    } else {
        m[0][0] *= x;
        m[0][1] *= x;
        m[0][2] *= x;
        m[0][3] *= x;
        m[1][0] *= y;
        m[1][1] *= y;
        m[1][2] *= y;
        m[1][3] *= y;
    }
    flagBits |= Scale;
}

void DMatrix4x4::scale(double x, double y, double z) {
    if (flagBits < Scale) {
        m[0][0] = x;
        m[1][1] = y;
        m[2][2] = z;
    } else if (flagBits < Rotation2D) {
        m[0][0] *= x;
        m[1][1] *= y;
        m[2][2] *= z;
    } else if (flagBits < Rotation) {
        m[0][0] *= x;
        m[0][1] *= x;
        m[1][0] *= y;
        m[1][1] *= y;
        m[2][2] *= z;
    } else {
        m[0][0] *= x;
        m[0][1] *= x;
        m[0][2] *= x;
        m[0][3] *= x;
        m[1][0] *= y;
        m[1][1] *= y;
        m[1][2] *= y;
        m[1][3] *= y;
        m[2][0] *= z;
        m[2][1] *= z;
        m[2][2] *= z;
        m[2][3] *= z;
    }
    flagBits |= Scale;
}

void DMatrix4x4::scale(double factor) {
    if (flagBits < Scale) {
        m[0][0] = factor;
        m[1][1] = factor;
        m[2][2] = factor;
    } else if (flagBits < Rotation2D) {
        m[0][0] *= factor;
        m[1][1] *= factor;
        m[2][2] *= factor;
    } else if (flagBits < Rotation) {
        m[0][0] *= factor;
        m[0][1] *= factor;
        m[1][0] *= factor;
        m[1][1] *= factor;
        m[2][2] *= factor;
    } else {
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
    }
    flagBits |= Scale;
}

void DMatrix4x4::translate(const DVector3D &vector) {
    double vx = vector.x();
    double vy = vector.y();
    double vz = vector.z();
    if (flagBits == Identity) {
        m[3][0] = vx;
        m[3][1] = vy;
        m[3][2] = vz;
    } else if (flagBits == Translation) {
        m[3][0] += vx;
        m[3][1] += vy;
        m[3][2] += vz;
    } else if (flagBits == Scale) {
        m[3][0] = m[0][0] * vx;
        m[3][1] = m[1][1] * vy;
        m[3][2] = m[2][2] * vz;
    } else if (flagBits == (Translation | Scale)) {
        m[3][0] += m[0][0] * vx;
        m[3][1] += m[1][1] * vy;
        m[3][2] += m[2][2] * vz;
    } else if (flagBits < Rotation) {
        m[3][0] += m[0][0] * vx + m[1][0] * vy;
        m[3][1] += m[0][1] * vx + m[1][1] * vy;
        m[3][2] += m[2][2] * vz;
    } else {
        m[3][0] += m[0][0] * vx + m[1][0] * vy + m[2][0] * vz;
        m[3][1] += m[0][1] * vx + m[1][1] * vy + m[2][1] * vz;
        m[3][2] += m[0][2] * vx + m[1][2] * vy + m[2][2] * vz;
        m[3][3] += m[0][3] * vx + m[1][3] * vy + m[2][3] * vz;
    }
    flagBits |= Translation;
}

void DMatrix4x4::translate(double x, double y) {
    if (flagBits == Identity) {
        m[3][0] = x;
        m[3][1] = y;
    } else if (flagBits == Translation) {
        m[3][0] += x;
        m[3][1] += y;
    } else if (flagBits == Scale) {
        m[3][0] = m[0][0] * x;
        m[3][1] = m[1][1] * y;
    } else if (flagBits == (Translation | Scale)) {
        m[3][0] += m[0][0] * x;
        m[3][1] += m[1][1] * y;
    } else if (flagBits < Rotation) {
        m[3][0] += m[0][0] * x + m[1][0] * y;
        m[3][1] += m[0][1] * x + m[1][1] * y;
    } else {
        m[3][0] += m[0][0] * x + m[1][0] * y;
        m[3][1] += m[0][1] * x + m[1][1] * y;
        m[3][2] += m[0][2] * x + m[1][2] * y;
        m[3][3] += m[0][3] * x + m[1][3] * y;
    }
    flagBits |= Translation;
}

void DMatrix4x4::translate(double x, double y, double z) {
    if (flagBits == Identity) {
        m[3][0] = x;
        m[3][1] = y;
        m[3][2] = z;
    } else if (flagBits == Translation) {
        m[3][0] += x;
        m[3][1] += y;
        m[3][2] += z;
    } else if (flagBits == Scale) {
        m[3][0] = m[0][0] * x;
        m[3][1] = m[1][1] * y;
        m[3][2] = m[2][2] * z;
    } else if (flagBits == (Translation | Scale)) {
        m[3][0] += m[0][0] * x;
        m[3][1] += m[1][1] * y;
        m[3][2] += m[2][2] * z;
    } else if (flagBits < Rotation) {
        m[3][0] += m[0][0] * x + m[1][0] * y;
        m[3][1] += m[0][1] * x + m[1][1] * y;
        m[3][2] += m[2][2] * z;
    } else {
        m[3][0] += m[0][0] * x + m[1][0] * y + m[2][0] * z;
        m[3][1] += m[0][1] * x + m[1][1] * y + m[2][1] * z;
        m[3][2] += m[0][2] * x + m[1][2] * y + m[2][2] * z;
        m[3][3] += m[0][3] * x + m[1][3] * y + m[2][3] * z;
    }
    flagBits |= Translation;
}

void DMatrix4x4::rotate(double angle, const DVector3D &vector) {
    rotate(angle, vector.x(), vector.y(), vector.z());
}

void DMatrix4x4::rotate(double angle, double x, double y, double z) {
    if (angle == 0.0f) return;
    double c, s;
    if (angle == 90.0f || angle == -270.0f) {
        s = 1.0f;
        c = 0.0f;
    } else if (angle == -90.0f || angle == 270.0f) {
        s = -1.0f;
        c = 0.0f;
    } else if (angle == 180.0f || angle == -180.0f) {
        s = 0.0f;
        c = -1.0f;
    } else {
        double a = MathUtil::DegreeToRadian(angle);
        c = std::cos(a);
        s = std::sin(a);
    }
    if (x == 0.0f) {
        if (y == 0.0f) {
            if (z != 0.0f) {
                // Rotate around the Z axis.
                if (z < 0) s = -s;
                double tmp;
                m[0][0] = (tmp = m[0][0]) * c + m[1][0] * s;
                m[1][0] = m[1][0] * c - tmp * s;
                m[0][1] = (tmp = m[0][1]) * c + m[1][1] * s;
                m[1][1] = m[1][1] * c - tmp * s;
                m[0][2] = (tmp = m[0][2]) * c + m[1][2] * s;
                m[1][2] = m[1][2] * c - tmp * s;
                m[0][3] = (tmp = m[0][3]) * c + m[1][3] * s;
                m[1][3] = m[1][3] * c - tmp * s;

                flagBits |= Rotation2D;
                return;
            }
        } else if (z == 0.0f) {
            // Rotate around the Y axis.
            if (y < 0) s = -s;
            double tmp;
            m[2][0] = (tmp = m[2][0]) * c + m[0][0] * s;
            m[0][0] = m[0][0] * c - tmp * s;
            m[2][1] = (tmp = m[2][1]) * c + m[0][1] * s;
            m[0][1] = m[0][1] * c - tmp * s;
            m[2][2] = (tmp = m[2][2]) * c + m[0][2] * s;
            m[0][2] = m[0][2] * c - tmp * s;
            m[2][3] = (tmp = m[2][3]) * c + m[0][3] * s;
            m[0][3] = m[0][3] * c - tmp * s;

            flagBits |= Rotation;
            return;
        }
    } else if (y == 0.0f && z == 0.0f) {
        // Rotate around the X axis.
        if (x < 0) s = -s;
        double tmp;
        m[1][0] = (tmp = m[1][0]) * c + m[2][0] * s;
        m[2][0] = m[2][0] * c - tmp * s;
        m[1][1] = (tmp = m[1][1]) * c + m[2][1] * s;
        m[2][1] = m[2][1] * c - tmp * s;
        m[1][2] = (tmp = m[1][2]) * c + m[2][2] * s;
        m[2][2] = m[2][2] * c - tmp * s;
        m[1][3] = (tmp = m[1][3]) * c + m[2][3] * s;
        m[2][3] = m[2][3] * c - tmp * s;

        flagBits |= Rotation;
        return;
    }

    double len = double(x) * double(x) + double(y) * double(y) + double(z) * double(z);
    if (!MathUtil::FuzzyIsEqual(len, 1.0) && !MathUtil::FuzzyIsNull(len)) {
        len = std::sqrt(len);
        x = double(double(x) / len);
        y = double(double(y) / len);
        z = double(double(z) / len);
    }
    double ic = 1.0f - c;
    DMatrix4x4 rot(Initialization::Uninitialized);
    rot.m[0][0] = x * x * ic + c;
    rot.m[1][0] = x * y * ic - z * s;
    rot.m[2][0] = x * z * ic + y * s;
    rot.m[3][0] = 0.0f;
    rot.m[0][1] = y * x * ic + z * s;
    rot.m[1][1] = y * y * ic + c;
    rot.m[2][1] = y * z * ic - x * s;
    rot.m[3][1] = 0.0f;
    rot.m[0][2] = x * z * ic - y * s;
    rot.m[1][2] = y * z * ic + x * s;
    rot.m[2][2] = z * z * ic + c;
    rot.m[3][2] = 0.0f;
    rot.m[0][3] = 0.0f;
    rot.m[1][3] = 0.0f;
    rot.m[2][3] = 0.0f;
    rot.m[3][3] = 1.0f;
    rot.flagBits = Rotation;
    *this *= rot;
}

void DMatrix4x4::rotate(const DQuaternion &quaternion) {
    DMatrix4x4 m(Initialization::Uninitialized);

    const double f2x = quaternion.x() + quaternion.x();
    const double f2y = quaternion.y() + quaternion.y();
    const double f2z = quaternion.z() + quaternion.z();
    const double f2xw = f2x * quaternion.scalar();
    const double f2yw = f2y * quaternion.scalar();
    const double f2zw = f2z * quaternion.scalar();
    const double f2xx = f2x * quaternion.x();
    const double f2xy = f2x * quaternion.y();
    const double f2xz = f2x * quaternion.z();
    const double f2yy = f2y * quaternion.y();
    const double f2yz = f2y * quaternion.z();
    const double f2zz = f2z * quaternion.z();

    m.m[0][0] = 1.0f - (f2yy + f2zz);
    m.m[1][0] = f2xy - f2zw;
    m.m[2][0] = f2xz + f2yw;
    m.m[3][0] = 0.0f;
    m.m[0][1] = f2xy + f2zw;
    m.m[1][1] = 1.0f - (f2xx + f2zz);
    m.m[2][1] = f2yz - f2xw;
    m.m[3][1] = 0.0f;
    m.m[0][2] = f2xz - f2yw;
    m.m[1][2] = f2yz + f2xw;
    m.m[2][2] = 1.0f - (f2xx + f2yy);
    m.m[3][2] = 0.0f;
    m.m[0][3] = 0.0f;
    m.m[1][3] = 0.0f;
    m.m[2][3] = 0.0f;
    m.m[3][3] = 1.0f;
    m.flagBits = Rotation;
    *this *= m;
}

void DMatrix4x4::ortho(double left, double right, double bottom, double top, double nearPlane,
                       double farPlane) {
    // Bail out if the projection volume is zero-sized.
    if (left == right || bottom == top || nearPlane == farPlane) return;

    // Construct the projection.
    double width = right - left;
    double invheight = top - bottom;
    double clip = farPlane - nearPlane;
    DMatrix4x4 m(Initialization::Uninitialized);
    m.m[0][0] = 2.0f / width;
    m.m[1][0] = 0.0f;
    m.m[2][0] = 0.0f;
    m.m[3][0] = -(left + right) / width;
    m.m[0][1] = 0.0f;
    m.m[1][1] = 2.0f / invheight;
    m.m[2][1] = 0.0f;
    m.m[3][1] = -(top + bottom) / invheight;
    m.m[0][2] = 0.0f;
    m.m[1][2] = 0.0f;
    m.m[2][2] = -2.0f / clip;
    m.m[3][2] = -(nearPlane + farPlane) / clip;
    m.m[0][3] = 0.0f;
    m.m[1][3] = 0.0f;
    m.m[2][3] = 0.0f;
    m.m[3][3] = 1.0f;
    m.flagBits = Translation | Scale;

    // Apply the projection.
    *this *= m;
}

void DMatrix4x4::frustum(double left, double right, double bottom, double top, double nearPlane,
                         double farPlane) {
    // Bail out if the projection volume is zero-sized.
    if (left == right || bottom == top || nearPlane == farPlane) return;

    // Construct the projection.
    DMatrix4x4 m(Initialization::Uninitialized);
    double width = right - left;
    double invheight = top - bottom;
    double clip = farPlane - nearPlane;
    m.m[0][0] = 2.0f * nearPlane / width;
    m.m[1][0] = 0.0f;
    m.m[2][0] = (left + right) / width;
    m.m[3][0] = 0.0f;
    m.m[0][1] = 0.0f;
    m.m[1][1] = 2.0f * nearPlane / invheight;
    m.m[2][1] = (top + bottom) / invheight;
    m.m[3][1] = 0.0f;
    m.m[0][2] = 0.0f;
    m.m[1][2] = 0.0f;
    m.m[2][2] = -(nearPlane + farPlane) / clip;
    m.m[3][2] = -2.0f * nearPlane * farPlane / clip;
    m.m[0][3] = 0.0f;
    m.m[1][3] = 0.0f;
    m.m[2][3] = -1.0f;
    m.m[3][3] = 0.0f;
    m.flagBits = General;

    // Apply the projection.
    *this *= m;
}

void DMatrix4x4::perspective(double verticalAngle, double aspectRatio, double nearPlane, double farPlane) {
    // Bail out if the projection volume is zero-sized.
    if (nearPlane == farPlane || aspectRatio == 0.0f) return;

    // Construct the projection.
    DMatrix4x4 m(Initialization::Uninitialized);
    double radians = MathUtil::DegreeToRadian(verticalAngle / 2.0f);
    double sine = std::sin(radians);
    if (sine == 0.0f) return;
    double cotan = std::cos(radians) / sine;
    double clip = farPlane - nearPlane;
    m.m[0][0] = cotan / aspectRatio;
    m.m[1][0] = 0.0f;
    m.m[2][0] = 0.0f;
    m.m[3][0] = 0.0f;
    m.m[0][1] = 0.0f;
    m.m[1][1] = cotan;
    m.m[2][1] = 0.0f;
    m.m[3][1] = 0.0f;
    m.m[0][2] = 0.0f;
    m.m[1][2] = 0.0f;
    m.m[2][2] = -(nearPlane + farPlane) / clip;
    m.m[3][2] = -(2.0f * nearPlane * farPlane) / clip;
    m.m[0][3] = 0.0f;
    m.m[1][3] = 0.0f;
    m.m[2][3] = -1.0f;
    m.m[3][3] = 0.0f;
    m.flagBits = General;

    // Apply the projection.
    *this *= m;
}

void DMatrix4x4::lookAt(const DVector3D &eye, const DVector3D &center, const DVector3D &up) {
    DVector3D forward = center - eye;
    if (MathUtil::FuzzyIsNull(forward.x()) && MathUtil::FuzzyIsNull(forward.y()) &&
        MathUtil::FuzzyIsNull(forward.z()))
        return;

    forward.normalize();
    DVector3D side = DVector3D::crossProduct(forward, up).normalized();
    DVector3D upVector = DVector3D::crossProduct(side, forward);

    DMatrix4x4 m(Initialization::Uninitialized);
    m.m[0][0] = side.x();
    m.m[1][0] = side.y();
    m.m[2][0] = side.z();
    m.m[3][0] = 0.0f;
    m.m[0][1] = upVector.x();
    m.m[1][1] = upVector.y();
    m.m[2][1] = upVector.z();
    m.m[3][1] = 0.0f;
    m.m[0][2] = -forward.x();
    m.m[1][2] = -forward.y();
    m.m[2][2] = -forward.z();
    m.m[3][2] = 0.0f;
    m.m[0][3] = 0.0f;
    m.m[1][3] = 0.0f;
    m.m[2][3] = 0.0f;
    m.m[3][3] = 1.0f;
    m.flagBits = Rotation;

    *this *= m;
    translate(-eye);
}

void DMatrix4x4::viewport(double left, double bottom, double width, double height, double nearPlane,
                          double farPlane) {
    const double w2 = width / 2.0f;
    const double h2 = height / 2.0f;

    DMatrix4x4 m(Initialization::Uninitialized);
    m.m[0][0] = w2;
    m.m[1][0] = 0.0f;
    m.m[2][0] = 0.0f;
    m.m[3][0] = left + w2;
    m.m[0][1] = 0.0f;
    m.m[1][1] = h2;
    m.m[2][1] = 0.0f;
    m.m[3][1] = bottom + h2;
    m.m[0][2] = 0.0f;
    m.m[1][2] = 0.0f;
    m.m[2][2] = (farPlane - nearPlane) / 2.0f;
    m.m[3][2] = (nearPlane + farPlane) / 2.0f;
    m.m[0][3] = 0.0f;
    m.m[1][3] = 0.0f;
    m.m[2][3] = 0.0f;
    m.m[3][3] = 1.0f;
    m.flagBits = General;

    *this *= m;
}

void DMatrix4x4::flipCoordinates() {
    if (flagBits < Rotation2D) {
        // Translation | Scale
        m[1][1] = -m[1][1];
        m[2][2] = -m[2][2];
    } else {
        m[1][0] = -m[1][0];
        m[1][1] = -m[1][1];
        m[1][2] = -m[1][2];
        m[1][3] = -m[1][3];
        m[2][0] = -m[2][0];
        m[2][1] = -m[2][1];
        m[2][2] = -m[2][2];
        m[2][3] = -m[2][3];
    }
    flagBits |= Scale;
}

void DMatrix4x4::copyDataTo(double *values) const {
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) { values[row * 4 + col] = double(m[col][row]); }
    }
}

void DMatrix4x4::optimize() {
    // If the last row is not (0, 0, 0, 1), the matrix is not a special type.
    flagBits = General;
    if (m[0][3] != 0 || m[1][3] != 0 || m[2][3] != 0 || m[3][3] != 1) return;

    flagBits &= ~Perspective;

    // If the last column is (0, 0, 0, 1), then there is no translation.
    if (m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0) flagBits &= ~Translation;

    // If the two first elements of row 3 and column 3 are 0, then any rotation must be about Z.
    if (!m[0][2] && !m[1][2] && !m[2][0] && !m[2][1]) {
        flagBits &= ~Rotation;
        // If the six non-diagonal elements in the top left 3x3 matrix are 0, there is no rotation.
        if (!m[0][1] && !m[1][0]) {
            flagBits &= ~Rotation2D;
            // Check for identity.
            if (m[0][0] == 1 && m[1][1] == 1 && m[2][2] == 1) flagBits &= ~Scale;
        } else {
            // If the columns are orthonormal and form a right-handed system, then there is no scale.
            double mm[4][4];
            copyToDoubles(m, mm);
            double det = matrixDet2(mm, 0, 1, 0, 1);
            double lenX = mm[0][0] * mm[0][0] + mm[0][1] * mm[0][1];
            double lenY = mm[1][0] * mm[1][0] + mm[1][1] * mm[1][1];
            double lenZ = mm[2][2];
            if (MathUtil::FuzzyIsEqual(det, 1.0) && MathUtil::FuzzyIsEqual(lenX, 1.0) &&
                MathUtil::FuzzyIsEqual(lenY, 1.0) && MathUtil::FuzzyIsEqual(lenZ, 1.0)) {
                flagBits &= ~Scale;
            }
        }
    } else {
        // If the columns are orthonormal and form a right-handed system, then there is no scale.
        double mm[4][4];
        copyToDoubles(m, mm);
        double det = matrixDet3(mm, 0, 1, 2, 0, 1, 2);
        double lenX = mm[0][0] * mm[0][0] + mm[0][1] * mm[0][1] + mm[0][2] * mm[0][2];
        double lenY = mm[1][0] * mm[1][0] + mm[1][1] * mm[1][1] + mm[1][2] * mm[1][2];
        double lenZ = mm[2][0] * mm[2][0] + mm[2][1] * mm[2][1] + mm[2][2] * mm[2][2];
        if (MathUtil::FuzzyIsEqual(det, 1.0) && MathUtil::FuzzyIsEqual(lenX, 1.0) &&
            MathUtil::FuzzyIsEqual(lenY, 1.0) && MathUtil::FuzzyIsEqual(lenZ, 1.0)) {
            flagBits &= ~Scale;
        }
    }
}

void DMatrix4x4::projectedRotate(double angle, double x, double y, double z) {
    // Used by QGraphicsRotation::applyTo() to perform a rotation
    // and projection back to 2D in a single step.
    if (angle == 0.0f) return;
    double c, s;
    if (angle == 90.0f || angle == -270.0f) {
        s = 1.0f;
        c = 0.0f;
    } else if (angle == -90.0f || angle == 270.0f) {
        s = -1.0f;
        c = 0.0f;
    } else if (angle == 180.0f || angle == -180.0f) {
        s = 0.0f;
        c = -1.0f;
    } else {
        double a = MathUtil::DegreeToRadian(angle);
        c = std::cos(a);
        s = std::sin(a);
    }
    if (x == 0.0f) {
        if (y == 0.0f) {
            if (z != 0.0f) {
                // Rotate around the Z axis.
                if (z < 0) s = -s;
                double tmp;
                m[0][0] = (tmp = m[0][0]) * c + m[1][0] * s;
                m[1][0] = m[1][0] * c - tmp * s;
                m[0][1] = (tmp = m[0][1]) * c + m[1][1] * s;
                m[1][1] = m[1][1] * c - tmp * s;
                m[0][2] = (tmp = m[0][2]) * c + m[1][2] * s;
                m[1][2] = m[1][2] * c - tmp * s;
                m[0][3] = (tmp = m[0][3]) * c + m[1][3] * s;
                m[1][3] = m[1][3] * c - tmp * s;

                flagBits |= Rotation2D;
                return;
            }
        } else if (z == 0.0f) {
            // Rotate around the Y axis.
            if (y < 0) s = -s;
            m[0][0] = m[0][0] * c + m[3][0] * s * inv_dist_to_plane;
            m[0][1] = m[0][1] * c + m[3][1] * s * inv_dist_to_plane;
            m[0][2] = m[0][2] * c + m[3][2] * s * inv_dist_to_plane;
            m[0][3] = m[0][3] * c + m[3][3] * s * inv_dist_to_plane;
            flagBits = General;
            return;
        }
    } else if (y == 0.0f && z == 0.0f) {
        // Rotate around the X axis.
        if (x < 0) s = -s;
        m[1][0] = m[1][0] * c - m[3][0] * s * inv_dist_to_plane;
        m[1][1] = m[1][1] * c - m[3][1] * s * inv_dist_to_plane;
        m[1][2] = m[1][2] * c - m[3][2] * s * inv_dist_to_plane;
        m[1][3] = m[1][3] * c - m[3][3] * s * inv_dist_to_plane;
        flagBits = General;
        return;
    }
    double len = double(x) * double(x) + double(y) * double(y) + double(z) * double(z);
    if (!MathUtil::FuzzyIsEqual(len, 1.0) && !MathUtil::FuzzyIsNull(len)) {
        len = std::sqrt(len);
        x = double(double(x) / len);
        y = double(double(y) / len);
        z = double(double(z) / len);
    }
    double ic = 1.0f - c;
    DMatrix4x4 rot(Initialization::Uninitialized);
    rot.m[0][0] = x * x * ic + c;
    rot.m[1][0] = x * y * ic - z * s;
    rot.m[2][0] = 0.0f;
    rot.m[3][0] = 0.0f;
    rot.m[0][1] = y * x * ic + z * s;
    rot.m[1][1] = y * y * ic + c;
    rot.m[2][1] = 0.0f;
    rot.m[3][1] = 0.0f;
    rot.m[0][2] = 0.0f;
    rot.m[1][2] = 0.0f;
    rot.m[2][2] = 1.0f;
    rot.m[3][2] = 0.0f;
    rot.m[0][3] = (x * z * ic - y * s) * -inv_dist_to_plane;
    rot.m[1][3] = (y * z * ic + x * s) * -inv_dist_to_plane;
    rot.m[2][3] = 0.0f;
    rot.m[3][3] = 1.0f;
    rot.flagBits = General;
    *this *= rot;
}

DMatrix4x4 DMatrix4x4::orthonormalInverse() const {
    DMatrix4x4 result(Initialization::Uninitialized);

    result.m[0][0] = m[0][0];
    result.m[1][0] = m[0][1];
    result.m[2][0] = m[0][2];

    result.m[0][1] = m[1][0];
    result.m[1][1] = m[1][1];
    result.m[2][1] = m[1][2];

    result.m[0][2] = m[2][0];
    result.m[1][2] = m[2][1];
    result.m[2][2] = m[2][2];

    result.m[0][3] = 0.0f;
    result.m[1][3] = 0.0f;
    result.m[2][3] = 0.0f;

    result.m[3][0] = -(result.m[0][0] * m[3][0] + result.m[1][0] * m[3][1] + result.m[2][0] * m[3][2]);
    result.m[3][1] = -(result.m[0][1] * m[3][0] + result.m[1][1] * m[3][1] + result.m[2][1] * m[3][2]);
    result.m[3][2] = -(result.m[0][2] * m[3][0] + result.m[1][2] * m[3][1] + result.m[2][2] * m[3][2]);
    result.m[3][3] = 1.0f;

    result.flagBits = flagBits;

    return result;
}
}  // namespace geometry
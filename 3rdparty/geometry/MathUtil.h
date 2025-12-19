#ifndef _MathUtil_H_
#define _MathUtil_H_

#include <stdlib.h>

#include <cmath>
#include <type_traits>

namespace geometry {

#ifndef CBD_M_PI
#define CBD_M_PI 3.14159265358979323846264338327950288
#endif

#ifndef CBD_M_2_PI
#define CBD_M_2_PI 6.2831853071795864769252867665590057
#endif

#ifndef CBD_M_PI_2
#define CBD_M_PI_2 1.57079632679489661923132169163975144
#endif

#ifndef CBD_M_PI_4
#define CBD_M_PI_4 0.785398163397448309615660845819875721
#endif

#ifndef CBD_M_LN10
#define CBD_M_LN10 2.30258509299404568401799145468436421
#endif

#ifndef CBD_M_LN2
#define CBD_M_LN2 0.693147180559945309417232121458176568
#endif

#define CBD_FLOAT_TOLERANCE 1e-5f
#define CBD_DOUBLE_TOLERANCE 1e-12
enum Initialization { Uninitialized };

class MathUtil {
public:
    template <typename _Ty>
    static inline bool IsNull(const _Ty &value);

    static inline bool FuzzyIsNull(float value);
    static inline bool FuzzyIsNull(double value);

    template <typename _Ty>
    static inline bool IsEqual(const _Ty &lv, const _Ty &rv);

    static inline bool FuzzyIsEqual(double lv, double rv);
    static inline bool FuzzyIsEqual(float lv, float rv);

    static inline double DegreeToRadian(double degree);
    static inline double RadianToDegree(double radian);

    static float FixRadian_0_2PI(float rad);
    static float FixRadian_NPI_PPI(float rad);
    static float FixAngle_0_360(float rad);
    static float FixAngle_N180_P180(float rad);
};

template <typename _Ty>
inline bool MathUtil::IsNull(const _Ty &value) {
    return value == _Ty(0);
}

bool MathUtil::FuzzyIsNull(double value) { return std::abs(value) <= CBD_DOUBLE_TOLERANCE; }

inline bool MathUtil::FuzzyIsNull(float value) { return std::abs(value) <= CBD_FLOAT_TOLERANCE; }

template <typename _Ty>
inline bool MathUtil::IsEqual(const _Ty &lv, const _Ty &rv) {
    return lv == rv;
}

inline bool MathUtil::FuzzyIsEqual(double lv, double rv) { return MathUtil::FuzzyIsNull(lv - rv); }

inline bool MathUtil::FuzzyIsEqual(float lv, float rv) { return MathUtil::FuzzyIsNull(lv - rv); }

inline double MathUtil::DegreeToRadian(double degree) { return degree * CBD_M_PI / 180.0; }

inline double MathUtil::RadianToDegree(double radian) { return radian * 180.0 / CBD_M_PI; }

}  // namespace geometry

#endif  // _MathUtil_H_

#include "MathUtil.h"

#include <math.h>

#include <cmath>
namespace geometry {
float geometry::MathUtil::FixRadian_0_2PI(float rad) {
#if defined(__linux__) || defined(__unix__) || defined(linux) || defined(__linux)
    float ret = fmodf(rad, CBD_M_2_PI);
#else
    float ret = std::fmodf(rad, CBD_M_2_PI);
#endif
    return (ret < 0) ? (ret + CBD_M_2_PI) : ret;
}

float geometry::MathUtil::FixRadian_NPI_PPI(float rad) {
#if defined(__linux__) || defined(__unix__) || defined(linux) || defined(__linux)
    float ret = fmodf(rad, CBD_M_2_PI);
#else
    float ret = std::fmodf(rad, CBD_M_2_PI);
#endif
    return (ret < -CBD_M_PI) ? (ret + CBD_M_2_PI) : ((ret > CBD_M_PI) ? (ret - CBD_M_2_PI) : ret);
}

float geometry::MathUtil::FixAngle_0_360(float rad) {
#if defined(__linux__) || defined(__unix__) || defined(linux) || defined(__linux)
    float ret = fmodf(rad, 360);
#else
    float ret = std::fmodf(rad, 360);
#endif
    return (ret < 0) ? (ret + 360) : ret;
}

float geometry::MathUtil::FixAngle_N180_P180(float rad) {
#if defined(__linux__) || defined(__unix__) || defined(linux) || defined(__linux)
    float ret = fmodf(rad, 360);
#else
    float ret = std::fmodf(rad, 360);
#endif
    return (ret < -180) ? (ret + 360) : ((ret > 180) ? (ret - 360) : ret);
}
}  // namespace geometry

#ifndef _BoundboxH_
#define _BoundboxH_

#include "geometry/FVector3D.h"
namespace geometry {
// 3D空间的boundbox
class BoundBox {
public:
    BoundBox();
    BoundBox(const FVector3D& pnt);
    BoundBox(const FVector3D& pnt0, const FVector3D& pnt1, bool ordered = false);
    /**
     * @brief 判断两个几何体是否有交叠，没有就返回false，有就返回true
     * @param[in] box 与当前几何是否有交叠的3D几何体
     * @return  有交叠返回true，没有交叠返回false
     */
    bool overlap(const BoundBox& box, float tol = 0.f) const;
    bool overlap(const BoundBox& otherbox, BoundBox& overlapbox) const;

    BoundBox& combine(const FVector3D& pnt);
    BoundBox& combine(const BoundBox& box);

    BoundBox& set(const FVector3D& pnt0, const FVector3D& pnt1, bool ordered = false);
    BoundBox& set(const FVector3D& pnt);

    bool contain(const FVector3D& pnt, float tol = 0.f) const;
    bool contain(const BoundBox& box, float tol = 0.f) const;
    void translate(const FVector3D& dir);
    BoundBox& offset(float offset);
    BoundBox offsetCpy(float offset) const;

    void reset();

    inline float xGap() const { return mMaxBound.x() - mMinBound.x(); }
    inline float yGap() const { return mMaxBound.y() - mMinBound.y(); }
    inline float zGap() const { return mMaxBound.z() - mMinBound.z(); }

    inline FVector3D center() const { return (mMinBound + mMaxBound) / 2; }

    // bottom surface : left-front, right-front, right-back, left-back;
    // top surface : same as bottom surface
    void getCorners(FVector3D corners[8]) const;

public:
    FVector3D mMinBound;
    FVector3D mMaxBound;
};
}  // namespace geometry
#endif  // _BoundboxH_

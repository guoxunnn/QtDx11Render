#ifndef _BoundBox2D_H_
#define _BoundBox2D_H_

#include <vector>
#include "geometry/FVector2D.h"

namespace geometry {
class BoundBox;
class BoundBox2D {
public:
    BoundBox2D();
    BoundBox2D(float xMin, float xMax, float yMin, float yMax);
    explicit BoundBox2D(const BoundBox& box);
    BoundBox2D(const FVector2D& pnt);
    BoundBox2D(const FVector3D& pnt);
    BoundBox2D(const FVector3D& pnt0, const FVector3D& pnt1, bool ordered = false);
    BoundBox2D(const FVector2D& pnt0, const FVector2D& pnt1, bool ordered = false);

    void reset();
    BoundBox2D& offset(float offset);
    BoundBox2D offsetCpy(float offset) const;

    // 根据传入的点，组成一个最大2D的盒子 如果传入的新点组成的盒子更大就更新，否则不变
    BoundBox2D& combine(const BoundBox& box);
    BoundBox2D& combine(const BoundBox2D& box2D);
    BoundBox2D& combine(const FVector3D& pnt);
    BoundBox2D& combine(const FVector2D& pnt);
    BoundBox2D& combine(const std::vector<FVector2D>& pnts);

    BoundBox2D& set(const FVector2D& pnt0, const FVector2D& pnt1, bool ordered = false);
    BoundBox2D& set(const FVector3D& pnt0, const FVector3D& pnt1, bool ordered = false);
    BoundBox2D& set(const FVector3D& pnt);
    BoundBox2D& set(const FVector2D& pnt);
    BoundBox2D& set(float xMin, float xMax, float yMin, float yMax);
    BoundBox2D& set(const std::vector<FVector2D>& pnts);
    BoundBox2D& set(const BoundBox& box);

    bool contain(const FVector2D& pnt, float tol = 0.f) const;
    bool contain(const BoundBox2D& box2D, float tol = 0.f) const;
    bool overlap(const BoundBox2D& box2D, float tol = 0.f) const;
    bool overlap(const BoundBox2D& otherBox, BoundBox2D& overlapBox) const;
    void translate(const FVector2D& dir);

    inline float xGap() const { return mMaxBound.x() - mMinBound.x(); }
    inline float yGap() const { return mMaxBound.y() - mMinBound.y(); }

    inline FVector2D center() const { return (mMinBound + mMaxBound) / 2; }

    inline float area() const {return (mMaxBound.x() - mMinBound.x()) * (mMaxBound.y() - mMinBound.y());}

    // left-bottom, right-bottom, right-top, left-top;
    void getCorners(FVector2D corners[4]) const;

public:
    FVector2D mMinBound;
    FVector2D mMaxBound;
};
}  // namespace geometry
#endif  // _BoundBox2D_H_

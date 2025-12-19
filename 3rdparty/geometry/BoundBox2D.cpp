#include "BoundBox2D.h"

#include <float.h>

#include "BoundBox.h"
namespace geometry {
BoundBox2D::BoundBox2D() {
    mMinBound.setX(FLT_MAX);
    mMinBound.setY(FLT_MAX);
    mMaxBound.setX(-FLT_MAX);
    mMaxBound.setY(-FLT_MAX);
}

BoundBox2D::BoundBox2D(float xMin, float xMax, float yMin, float yMax) {
    mMinBound.setX(xMin);
    mMinBound.setY(yMin);
    mMaxBound.setX(xMax);
    mMaxBound.setY(yMax);
}

BoundBox2D::BoundBox2D(const BoundBox &box) {
    mMinBound.setX(box.mMinBound.x());
    mMinBound.setY(box.mMinBound.y());
    mMaxBound.setX(box.mMaxBound.x());
    mMaxBound.setY(box.mMaxBound.y());
}

BoundBox2D::BoundBox2D(const FVector2D &pnt) {
    mMinBound.setX(pnt.x());
    mMinBound.setY(pnt.y());
    mMaxBound.setX(pnt.x());
    mMaxBound.setY(pnt.y());
}

BoundBox2D::BoundBox2D(const FVector3D &pnt) {
    mMinBound.setX(pnt.x());
    mMinBound.setY(pnt.y());
    mMaxBound.setX(pnt.x());
    mMaxBound.setY(pnt.y());
}

BoundBox2D::BoundBox2D(const FVector3D &pnt0, const FVector3D &pnt1, bool ordered) {
    if (ordered) {
        mMinBound.setX(pnt0.x());
        mMinBound.setY(pnt0.y());
        mMaxBound.setX(pnt1.x());
        mMaxBound.setY(pnt1.y());
    } else {
        mMinBound.setX(std::min(pnt0.x(), pnt1.x()));
        mMinBound.setY(std::min(pnt0.y(), pnt1.y()));
        mMaxBound.setX(std::max(pnt0.x(), pnt1.x()));
        mMaxBound.setY(std::max(pnt0.y(), pnt1.y()));
    }
}

BoundBox2D::BoundBox2D(const FVector2D &pnt0, const FVector2D &pnt1, bool ordered) {
    if (ordered) {
        mMinBound.setX(pnt0.x());
        mMinBound.setY(pnt0.y());
        mMaxBound.setX(pnt1.x());
        mMaxBound.setY(pnt1.y());
    } else {
        mMinBound.setX(std::min(pnt0.x(), pnt1.x()));
        mMinBound.setY(std::min(pnt0.y(), pnt1.y()));
        mMaxBound.setX(std::max(pnt0.x(), pnt1.x()));
        mMaxBound.setY(std::max(pnt0.y(), pnt1.y()));
    }
}

void BoundBox2D::reset() {
    mMinBound.setX(FLT_MAX);
    mMinBound.setY(FLT_MAX);
    mMaxBound.setX(-FLT_MAX);
    mMaxBound.setY(-FLT_MAX);
}

BoundBox2D &BoundBox2D::offset(float offset) {
    mMinBound.setX(mMinBound.x() - offset);
    mMinBound.setY(mMinBound.y() - offset);
    mMaxBound.setX(mMaxBound.x() + offset);
    mMaxBound.setY(mMaxBound.y() + offset);
    return *this;
}

BoundBox2D BoundBox2D::offsetCpy(float offset) const {
    BoundBox2D box2D(*this);
    box2D.offset(offset);
    return box2D;
}

BoundBox2D &BoundBox2D::combine(const BoundBox &box) {
    mMinBound.setX(std::min(mMinBound.x(), box.mMinBound.x()));
    mMinBound.setY(std::min(mMinBound.y(), box.mMinBound.y()));
    mMaxBound.setX(std::max(mMaxBound.x(), box.mMaxBound.x()));
    mMaxBound.setY(std::max(mMaxBound.y(), box.mMaxBound.y()));
    return *this;
}

BoundBox2D &BoundBox2D::combine(const BoundBox2D &box2D) {
    mMinBound.setX(std::min(mMinBound.x(), box2D.mMinBound.x()));
    mMinBound.setY(std::min(mMinBound.y(), box2D.mMinBound.y()));
    mMaxBound.setX(std::max(mMaxBound.x(), box2D.mMaxBound.x()));
    mMaxBound.setY(std::max(mMaxBound.y(), box2D.mMaxBound.y()));
    return *this;
}

BoundBox2D &BoundBox2D::combine(const FVector3D &pnt) {
    mMinBound.setX(std::min(mMinBound.x(), pnt.x()));
    mMinBound.setY(std::min(mMinBound.y(), pnt.y()));
    mMaxBound.setX(std::max(mMaxBound.x(), pnt.x()));
    mMaxBound.setY(std::max(mMaxBound.y(), pnt.y()));
    return *this;
}

BoundBox2D &BoundBox2D::combine(const FVector2D &pnt) {
    mMinBound.setX(std::min(mMinBound.x(), pnt.x()));
    mMinBound.setY(std::min(mMinBound.y(), pnt.y()));
    mMaxBound.setX(std::max(mMaxBound.x(), pnt.x()));
    mMaxBound.setY(std::max(mMaxBound.y(), pnt.y()));
    return *this;
}

BoundBox2D &BoundBox2D::combine(const std::vector<FVector2D> &pnts) {
    for (size_t i = 0; i < pnts.size(); i++) { combine(pnts[i]); }
    return *this;
}

BoundBox2D &BoundBox2D::set(const FVector2D &pnt0, const FVector2D &pnt1, bool ordered) {
    if (ordered) {
        mMinBound = pnt0;
        mMaxBound = pnt1;
    } else {
        mMinBound.setX(std::min(pnt0.x(), pnt1.x()));
        mMinBound.setY(std::min(pnt0.y(), pnt1.y()));
        mMaxBound.setX(std::max(pnt0.x(), pnt1.x()));
        mMaxBound.setY(std::max(pnt0.y(), pnt1.y()));
    }
    return *this;
}

BoundBox2D &BoundBox2D::set(const FVector3D &pnt0, const FVector3D &pnt1, bool ordered) {
    FVector2D p1(pnt0), p2(pnt1);
    return set((FVector2D)pnt0, (FVector2D)pnt1, ordered);
}

BoundBox2D &BoundBox2D::set(const FVector3D &pnt) {
    mMinBound.setX(pnt.x());
    mMinBound.setY(pnt.y());
    mMaxBound.setX(pnt.x());
    mMaxBound.setY(pnt.y());
    return *this;
}

BoundBox2D &BoundBox2D::set(float xMin, float xMax, float yMin, float yMax) {
    mMinBound.setX(xMin);
    mMinBound.setY(yMin);
    mMaxBound.setX(xMax);
    mMaxBound.setY(yMax);
    return *this;
}

BoundBox2D &BoundBox2D::set(const FVector2D &pnt) {
    mMinBound.setX(pnt.x());
    mMinBound.setY(pnt.y());
    mMaxBound.setX(pnt.x());
    mMaxBound.setY(pnt.y());
    return *this;
}

BoundBox2D &BoundBox2D::set(const std::vector<FVector2D> &pnts) {
    reset();
    for (size_t i = 0; i < pnts.size(); i++) { combine(pnts[i]); }
    return *this;
}

BoundBox2D &BoundBox2D::set(const BoundBox &box) {
    mMinBound.setX(box.mMinBound.x());
    mMinBound.setY(box.mMinBound.y());
    mMaxBound.setX(box.mMaxBound.x());
    mMaxBound.setY(box.mMaxBound.y());
    return *this;
}

bool BoundBox2D::contain(const FVector2D &pnt, float tol) const {
    return !(pnt.x() < mMinBound.x() - tol || pnt.x() > mMaxBound.x() + tol || pnt.y() < mMinBound.y() - tol ||
             pnt.y() > mMaxBound.y() + tol);
}

bool BoundBox2D::contain(const BoundBox2D &box2D, float tol) const {
    return (mMinBound.x() <= box2D.mMinBound.x() + tol && mMinBound.y() <= box2D.mMinBound.y() + tol &&
        mMaxBound.x() >= box2D.mMaxBound.x() - tol && mMaxBound.y() >= box2D.mMaxBound.y() - tol);
}

bool BoundBox2D::overlap(const BoundBox2D &box2D, float tol) const {
    return !(mMinBound.x() > box2D.mMaxBound.x() + tol || mMaxBound.x() < box2D.mMinBound.x() - tol ||
        mMinBound.y() > box2D.mMaxBound.y() + tol || mMaxBound.y() < box2D.mMinBound.y() - tol);
}

bool BoundBox2D::overlap(const BoundBox2D &otherBox, BoundBox2D &overlapBox) const {
    if (!overlap(otherBox)) { return false; }
    FVector2D &overlapMinBound = overlapBox.mMinBound;
    overlapMinBound.setX(std::max(mMinBound.x(), otherBox.mMinBound.x()));
    overlapMinBound.setY(std::max(mMinBound.y(), otherBox.mMinBound.y()));
    FVector2D &overlapMaxBound = overlapBox.mMaxBound;
    overlapMaxBound.setX(std::min(mMaxBound.x(), otherBox.mMaxBound.x()));
    overlapMaxBound.setY(std::min(mMaxBound.y(), otherBox.mMaxBound.y()));
    return true;
}

void BoundBox2D::translate(const FVector2D &dir) {
    mMinBound += dir;
    mMaxBound += dir;
}
void BoundBox2D::getCorners(FVector2D corners[4]) const {
    float xMin = mMinBound.x(), xMax = mMaxBound.x();
    float yMin = mMinBound.y(), yMax = mMaxBound.y();
    corners[0] = FVector2D(xMin, yMin);
    corners[1] = FVector2D(xMax, yMin);
    corners[2] = FVector2D(xMax, yMax);
    corners[3] = FVector2D(xMin, yMax);
}
}  // namespace geometry
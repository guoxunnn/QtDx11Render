#include "BoundBox.h"
#include <iostream>
#include <cmath>
#include <cfloat>

namespace geometry {
BoundBox::BoundBox() {
    mMinBound.setX(FLT_MAX);
    mMinBound.setY(FLT_MAX);
    mMinBound.setZ(FLT_MAX);
    mMaxBound.setX(-FLT_MAX);
    mMaxBound.setY(-FLT_MAX);
    mMaxBound.setZ(-FLT_MAX);
}

BoundBox::BoundBox(const FVector3D &pnt) {
    mMinBound.setX(pnt.x());
    mMinBound.setY(pnt.y());
    mMinBound.setZ(pnt.z());
    mMaxBound.setX(pnt.x());
    mMaxBound.setY(pnt.y());
    mMaxBound.setZ(pnt.z());
}

BoundBox::BoundBox(const FVector3D &pnt0, const FVector3D &pnt1, bool ordered) {
    if (ordered) {
        mMinBound = pnt0;
        mMaxBound = pnt1;
    } else {
        mMinBound.setX(std::min(pnt0.x(), pnt1.x()));
        mMinBound.setY(std::min(pnt0.y(), pnt1.y()));
        mMinBound.setZ(std::min(pnt0.z(), pnt1.z()));
        mMaxBound.setX(std::max(pnt0.x(), pnt1.x()));
        mMaxBound.setY(std::max(pnt0.y(), pnt1.y()));
        mMaxBound.setZ(std::max(pnt0.z(), pnt1.z()));
    }
}

bool BoundBox::overlap(const BoundBox &box, float tol) const {
    return !(mMinBound.x() > box.mMaxBound.x() + tol || mMaxBound.x() < box.mMinBound.x() - tol ||
        mMinBound.y() > box.mMaxBound.y() + tol || mMaxBound.y() < box.mMinBound.y() - tol ||
        mMinBound.z() > box.mMaxBound.z() + tol || mMaxBound.z() < box.mMinBound.z() - tol);
}

bool BoundBox::overlap(const BoundBox &otherbox, BoundBox &overlapbox) const {
    if (!overlap(otherbox)) { return false; }
    FVector3D &overlapMinBound = overlapbox.mMinBound;
    overlapMinBound.setX(std::max(mMinBound.x(), otherbox.mMinBound.x()));
    overlapMinBound.setY(std::max(mMinBound.y(), otherbox.mMinBound.y()));
    overlapMinBound.setZ(std::max(mMinBound.z(), otherbox.mMinBound.z()));
    FVector3D &overlapMaxBound = overlapbox.mMaxBound;
    overlapMaxBound.setX(std::min(mMaxBound.x(), otherbox.mMaxBound.x()));
    overlapMaxBound.setY(std::min(mMaxBound.y(), otherbox.mMaxBound.y()));
    overlapMaxBound.setZ(std::min(mMaxBound.z(), otherbox.mMaxBound.z()));
    return true;
}

BoundBox &BoundBox::combine(const FVector3D &pnt) {
    mMinBound.setX(std::min(mMinBound.x(), pnt.x()));
    mMinBound.setY(std::min(mMinBound.y(), pnt.y()));
    mMinBound.setZ(std::min(mMinBound.z(), pnt.z()));
    mMaxBound.setX(std::max(mMaxBound.x(), pnt.x()));
    mMaxBound.setY(std::max(mMaxBound.y(), pnt.y()));
    mMaxBound.setZ(std::max(mMaxBound.z(), pnt.z()));
    return *this;
}

BoundBox &BoundBox::combine(const BoundBox &box) {
    mMinBound.setX(std::min(mMinBound.x(), box.mMinBound.x()));
    mMinBound.setY(std::min(mMinBound.y(), box.mMinBound.y()));
    mMinBound.setZ(std::min(mMinBound.z(), box.mMinBound.z()));
    mMaxBound.setX(std::max(mMaxBound.x(), box.mMaxBound.x()));
    mMaxBound.setY(std::max(mMaxBound.y(), box.mMaxBound.y()));
    mMaxBound.setZ(std::max(mMaxBound.z(), box.mMaxBound.z()));
    return *this;
}

BoundBox &BoundBox::set(const FVector3D &pnt0, const FVector3D &pnt1, bool ordered) {
    if (ordered) {
        mMinBound = pnt0;
        mMaxBound = pnt1;
    } else {
        mMinBound.setX(std::min(pnt0.x(), pnt1.x()));
        mMinBound.setY(std::min(pnt0.y(), pnt1.y()));
        mMinBound.setZ(std::min(pnt0.z(), pnt1.z()));
        mMaxBound.setX(std::max(pnt0.x(), pnt1.x()));
        mMaxBound.setY(std::max(pnt0.y(), pnt1.y()));
        mMaxBound.setZ(std::max(pnt0.z(), pnt1.z()));
    }
    return *this;
}

BoundBox &BoundBox::set(const FVector3D &pnt) {
    mMinBound.setX(pnt.x());
    mMinBound.setY(pnt.y());
    mMinBound.setZ(pnt.z());
    mMaxBound.setX(pnt.x());
    mMaxBound.setY(pnt.y());
    mMaxBound.setZ(pnt.z());
    return *this;
}

bool BoundBox::contain(const FVector3D &pnt, float tol) const {
    return !(pnt.x() < mMinBound.x() - tol || pnt.x() > mMaxBound.x() + tol || pnt.y() < mMinBound.y() - tol ||
        pnt.y() > mMaxBound.y() + tol || pnt.z() < mMinBound.z() - tol || pnt.z() > mMaxBound.z() + tol);
}

bool BoundBox::contain(const BoundBox &box, float tol) const {
    return (mMinBound.x() <= box.mMinBound.x() + tol && mMinBound.y() <= box.mMinBound.y() + tol &&
            mMinBound.z() <= box.mMinBound.z() + tol && mMaxBound.x() >= box.mMaxBound.x() - tol &&
            mMaxBound.y() >= box.mMaxBound.y() - tol && mMaxBound.z() >= box.mMaxBound.z() - tol);
}

void BoundBox::translate(const FVector3D &dir) {
    mMinBound += dir;
    mMaxBound += dir;
}

BoundBox &BoundBox::offset(float offset) {
    mMinBound.setX(mMinBound.x() - offset);
    mMinBound.setY(mMinBound.y() - offset);
    mMinBound.setZ(mMinBound.z() - offset);
    mMaxBound.setX(mMaxBound.x() + offset);
    mMaxBound.setY(mMaxBound.y() + offset);
    mMaxBound.setZ(mMaxBound.z() + offset);
    return *this;
}

BoundBox BoundBox::offsetCpy(float offset) const {
    BoundBox box(*this);
    box.offset(offset);
    return box;
}

void BoundBox::reset() {
    mMinBound.setX(FLT_MAX);
    mMinBound.setY(FLT_MAX);
    mMinBound.setZ(FLT_MAX);
    mMaxBound.setX(-FLT_MAX);
    mMaxBound.setY(-FLT_MAX);
    mMaxBound.setZ(-FLT_MAX);
}

void BoundBox::getCorners(FVector3D corners[8]) const {
    float xMin = mMinBound.x(), xMax = mMaxBound.x();
    float yMin = mMinBound.y(), yMax = mMaxBound.y();
    float zMin = mMinBound.z(), zMax = mMaxBound.z();
    corners[0] = FVector3D(xMin, yMin, zMin);
    corners[1] = FVector3D(xMax, yMin, zMin);
    corners[2] = FVector3D(xMax, yMax, zMin);
    corners[3] = FVector3D(xMin, yMax, zMin);
    corners[4] = FVector3D(xMin, yMin, zMax);
    corners[5] = FVector3D(xMax, yMin, zMax);
    corners[6] = FVector3D(xMax, yMax, zMax);
    corners[7] = FVector3D(xMin, yMax, zMax);
}
}  // namespace geometry

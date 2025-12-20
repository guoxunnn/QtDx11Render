#include "RCamera.h"
#include "geometry/DQuaternion.h"
#include "geometry/FMatrix4x4.h"
#include <algorithm>

namespace render {
using namespace geometry;

static constexpr double Clip_Radius = 2500;
static constexpr double Clip_Max_Height = 2500;
static constexpr double Clip_Min_Height = 0.1;

// 射线
struct Ray {
    Ray(const geometry::FVector3D& ray_pos, const geometry::FVector3D& ray_dir, bool ray_bi_dir)
        : pos(ray_pos), dir(ray_dir), bi_dir(ray_bi_dir) {}
    geometry::FVector3D pos; // 射线原点
    geometry::FVector3D dir; // 射线方向
    bool bi_dir; // 是否为双向
};

RCamera::RCamera() : mode_(Orthometric) { Reset(); }

RCamera::~RCamera() {}

RCamera::RCamera(const RCamera& other) {
    CopyFrom(other);
}

RCamera& RCamera::operator=(const RCamera& other) {
    if (&other == this) {
        return *this;
    }
    CopyFrom(other);
    return *this;
}

void RCamera::CalcuFullVisibility(geometry::BoundBox box) {
    const geometry::DVector3D look_center = geometry::DVector3D(box.center().x(), box.center().y(), box.center().z());
    const geometry::DVector3D camera_forward = this->forward_;
    const geometry::DVector3D camera_up = up_;
    const geometry::DVector3D camera_right = DVector3D::crossProduct(forward_, up_).normalized();
    geometry::FMatrix4x4 mat(camera_right.x(), camera_right.y(), camera_right.z(), 0.f
        , camera_up.x(), camera_up.y(), camera_up.z(), 0.f
        , -camera_forward.x(), -camera_forward.y(), -camera_forward.z(), 0.f
        , 0.f, 0.f, 0.f, 1.f);
    int width = viewportWidth_;
    int height = viewportHeight_;
    FVector3D corners[8];
    box.getCorners(corners);

    geometry::BoundBox cal_box;
    for(int i = 0; i < 8; ++i) {
        cal_box.combine(mat * corners[i]);
    }

    auto up_len = cal_box.yGap() / 2;
    auto right_len = cal_box.xGap() / 2;
    const auto half_angle = 22.5 * 3.1415926 / 180;
    const auto up_value = std::tan(half_angle);
    const auto up_distance = up_len / up_value;
    const auto right_tan_value = (width / 2.0) * std::tan(half_angle) / (height / 2.0);
    const auto right_distance = right_len / right_tan_value;

    auto result = geometry::FVector3D(0, 0, 0) - cal_box.center();
    result.setZ(-(std::max(up_distance, right_distance) + cal_box.mMaxBound.z()));
    SetXYZDistance(-result.x(), -result.y(), -result.z());
}

void RCamera::SetProjectMode(ProjectMode mode) {
    std::lock_guard lg(lock_);
    if (mode != mode_) {
        if (mode == Orthometric) {
            double w, h;
            GetClipWidthAndHeight(w, h);
            clipHeight_ = h;
            distance_ = Clip_Radius;
        }
        else {
            double fov = MathUtil::DegreeToRadian(fov_);
            distance_ = clipHeight_ * 0.5 / std::tan(fov * 0.5);
        }
        mode_ = mode;
        ComputeProjMatrix();
        ComputeViewMatrix();
    }
}

RCamera::ProjectMode RCamera::GetProjectMode() const {
    std::lock_guard lg(lock_);
    return mode_;
}

void RCamera::SetZeroToOneDepth(bool is021) {
    std::lock_guard lg(lock_);
    isZeroToOneDepth_ = is021;
}

geometry::DVector3D RCamera::GetRightDirection() const {
    std::lock_guard lg(lock_);
    return DVector3D::crossProduct(forward_, up_);
}

double RCamera::GetDistance() const {
    std::lock_guard lg(lock_);
    return distance_;
}

void RCamera::SetRotationCenter(const geometry::DVector3D& center) {
    std::lock_guard lg(lock_);
    rotationCenter_ = center;
}

geometry::DVector3D RCamera::GetRotationCenter() const {
    std::lock_guard lg(lock_);
    return rotationCenter_;
}

void RCamera::ResetRotationCenter() {
    std::lock_guard lg(lock_);
    rotationCenter_ = DVector3D(0, 0, 0);
}

bool RCamera::IsZeroToOneDepth() const {
    std::lock_guard lg(lock_);
    return isZeroToOneDepth_;
}

void RCamera::GetXYZDistance(float& x, float& y, float& z) const {
    std::lock_guard lg(lock_);
    x = translation_.x();
    y = translation_.y();
    if (mode_ == Orthometric) {
        double fov = MathUtil::DegreeToRadian(fov_);
        double dis = clipHeight_ * 0.5 / std::tan(fov * 0.5);
        z = dis;
    }
    else {
        z = distance_;
    }
}

void RCamera::SetXYZDistance(float x, float y, float z) {
    std::lock_guard lg(lock_);
    if (mode_ == Orthometric) {
        translation_ = geometry::DVector2D(x, y);
        distance_ = z;
        double w, h;
        ComputeViewMatrix();
        GetClipWidthAndHeight(w, h, DVector3D(0, 0, 0), true);
        clipHeight_ = h;
        distance_ = Clip_Radius; // 还原
        ComputeViewMatrix();
    }
    else {
        translation_ = geometry::DVector2D(x, y);
        distance_ = z;
        ComputeViewMatrix();
    }
    ComputeProjMatrix();
}

// 以focus_为旋转点，绕view的right轴旋转
void RCamera::Pitch(double angle, bool base_rotation_center) {
    if (MathUtil::FuzzyIsNull(angle)) { return; }

    std::lock_guard lg(lock_);
    DVector3D right = DVector3D::crossProduct(forward_, up_).normalized();
    DQuaternion q = DQuaternion::fromAxisAndAngle(right, -angle);
    forward_ = q.rotatedVector(forward_).normalized();
    up_ = DVector3D::crossProduct(right, forward_).normalized();
    ComputeViewMatrix();
    if (base_rotation_center) {
        // 依赖新的模型视图矩阵，因此需在ComputeViewMatrix后调用
        UpdateTranslationForRotate(q);
    }
}

// 以focus_为旋转点，绕up的轴旋转
void RCamera::Yaw(double angle, bool base_rotation_center) {
    if (MathUtil::FuzzyIsNull(angle)) { return; }

    std::lock_guard lg(lock_);
    DQuaternion q = DQuaternion::fromAxisAndAngle(up_, -angle).normalized();
    forward_ = q.rotatedVector(forward_).normalized();
    ComputeViewMatrix();
    if (base_rotation_center) {
        // 依赖新的模型视图矩阵，因此需在ComputeViewMatrix后调用
        UpdateTranslationForRotate(q);
    }
}

// 以focus_为旋转点，绕平面法向旋转
void RCamera::Roll(double angle, bool base_rotation_center) {
    if (MathUtil::FuzzyIsNull(angle)) { return; }

    std::lock_guard lg(lock_);
    DQuaternion q = DQuaternion::fromAxisAndAngle(forward_, angle).normalized();
    up_ = q.rotatedVector(up_).normalized();
    ComputeViewMatrix();
    if (base_rotation_center) {
        // 依赖新的模型视图矩阵，因此需在ComputeViewMatrix后调用
        UpdateTranslationForRotate(q);
    }
}

void RCamera::RollOnWorld(double angle, bool base_rotation_center) {
    if (MathUtil::FuzzyIsNull(angle)) { return; }

    std::lock_guard lg(lock_);
    DQuaternion q = DQuaternion::fromAxisAndAngle(DVector3D(0, 0, 1), -angle).normalized();
    forward_ = q.rotatedVector(forward_).normalized();
    up_ = q.rotatedVector(up_).normalized();
    ComputeViewMatrix();
    if (base_rotation_center) {
        // 依赖新的模型视图矩阵，因此需在ComputeViewMatrix后调用
        UpdateTranslationForRotate(q);
    }
}

void RCamera::Pan(const geometry::DVector2D &offset) {
    std::lock_guard lg(lock_);
    translation_ -= offset;
    ComputeProjMatrix();
}

void RCamera::Zoom(double factor) {
    std::lock_guard lg(lock_);
    if (mode_ == Orthometric) {
        double new_height;
        if (CalculateZoomForOrtho(factor, new_height)) {
            clipHeight_ = new_height;
            ComputeProjMatrix();
        }
    }
    else {
        double new_distance;
        if(CalculateZoomForPersp(factor, new_distance)) {
            // 相似三角比例差
            distance_ = new_distance;
            ComputeViewMatrix();
            ComputeProjMatrix();
        }
    }
}

void RCamera::ZoomAtPoint(const geometry::DVector2D& mousePos, double factor) {
    std::lock_guard lg(lock_);
    DVector3D far_clip_pos = ViewportPointToFarClip(mousePos);
    DVector2D offset(far_clip_pos.x() - translation_.x(), far_clip_pos.y() - translation_.y());
    if (mode_ == Orthometric) {
        double new_height;
        if (CalculateZoomForOrtho(factor, new_height)) {
            clipHeight_ = new_height;

            offset *= (factor - 1) / factor;
            translation_ += offset;
            ComputeProjMatrix();
        }
    }
    else {
        double new_distance, old_distance = distance_;
        if (CalculateZoomForPersp(factor, new_distance)) {
            distance_ = new_distance;
            // 相似三角比例差
            double dif_fac = 1 - ((Clip_Radius + new_distance) / (Clip_Radius + old_distance));
            offset *= dif_fac;
            translation_ += offset;

            ComputeViewMatrix();
            ComputeProjMatrix();
        }
    }
}

bool computeRayPlaneIntersection(const Ray& ray, const geometry::FVector3D& v0,
    const geometry::FVector3D& v1, const geometry::FVector3D& v2, geometry::FVector3D& inter, float& distance) {
    distance = FLT_MAX;
    // compute the plane's normal
    FVector3D v0v1 = v1 - v0;
    FVector3D v0v2 = v2 - v0;
    // no need to normalize
    FVector3D N = FVector3D::crossProduct(v0v1, v0v2);  // N

    // Step 1: finding P

    // check if the ray and plane are parallel.
    float NdotRayDirection = FVector3D::dotProduct(N, ray.dir);
    if (std::fabs(NdotRayDirection) < 1e-6) {  // almost 0, they are parallel, so they don't intersect!
        return false;
    }

    // compute d parameter using equation 2
    float d = -FVector3D::dotProduct(N, v0);

    // compute distance (equation 3)
    distance = -(FVector3D::dotProduct(N, ray.pos) + d) / NdotRayDirection;

    // check if the triangle is behind the ray
    if (!ray.bi_dir && distance < 0) { // the triangle is behind
        return false;
    }

    //// compute the intersection point using equation 1
    inter = ray.pos + distance * ray.dir;
    return true;  // this ray hits the triangle
}

// 求射线与平面交点，有交返回true
// inter [out]: 交点坐标
// distance [out]: 交点与射线原点的距离，ray.bi_dir == true时可能计算为负值，说明交点位于射线原点后面
bool ComputeRayPlaneIntersection(const Ray& ray, const geometry::FVector3D& v0,
                                        const geometry::FVector3D& v1, const geometry::FVector3D& v2, geometry::FVector3D& inter) {
    float distance;
    return computeRayPlaneIntersection(ray, v0, v1, v2, inter, distance);
}

bool RCamera::GetRayInterTri(int px, int py, geometry::FVector3D tri[3], geometry::FVector3D& pos_at_tri) const {
    DVector3D clip_pos = ViewportPointToFarClip(DVector2D(px, py));
    DVector3D world_pos = ViewToWorld(clip_pos);
    DVector3D dir = GetRayDirection(world_pos);
    FVector3D normal = FVector3D::normal(tri[0], tri[1], tri[2]);
    Ray ray(world_pos.toFVector3D(), dir.toFVector3D(), true);
    if (std::abs(FVector3D::dotProduct(normal, ray.dir)) < 1e-6) {  // 平行于单元平面
        return false;
    }

    if (normal.isNull()) {
        return false;
    }
    if (!ComputeRayPlaneIntersection(ray, tri[0], tri[1],
        tri[2], pos_at_tri)) {
        return false;
    }
    return true;
}

void RCamera::ResizeViewport(int w, int h) {
    if (w <= 0 || h <= 0) { return; }

    std::lock_guard lg(lock_);
    viewportWidth_ = w;
    viewportHeight_ = h;

    double aspect = viewportWidth_ / viewportHeight_;
    if (!MathUtil::FuzzyIsEqual(clipAspect_, aspect)) {
        clipAspect_ = aspect;
        ComputeProjMatrix();
    }
}

int RCamera::GetViewPortWidth() const {
    return viewportWidth_;
}

int RCamera::GetViewPortHeight() const {
    return viewportHeight_;
}

geometry::DVector3D RCamera::ViewportPointToClip(const geometry::DVector2D& pos,
    const geometry::DVector3D& ref_world_point) const {

    std::lock_guard lg(lock_);
    double w, h;
    GetClipWidthAndHeight(w, h, ref_world_point);
    double r = h / viewportHeight_;
    double z_pos = WorldToView(ref_world_point).z();
    DVector3D leftBottom(translation_.x() - w * 0.5, translation_.y() - h * 0.5, z_pos);

    return leftBottom + DVector3D(pos.x() * r, (viewportHeight_ - pos.y()) * r, 0);
}

geometry::DVector3D RCamera::ViewportPointToNearClip(const geometry::DVector2D& pos) const {
    double near, far;
    GetClipNearAndFar(near, far);
    return ViewportPointToClip(pos, forward_ * (near - distance_));
}

geometry::DVector3D RCamera::ViewportPointToFarClip(const geometry::DVector2D& pos) const {
    double near, far;
    GetClipNearAndFar(near, far);
    return ViewportPointToClip(pos, forward_ * (far - distance_));
}

geometry::DVector3D RCamera::WorldToView(const geometry::DVector3D &pos) const {
    return GetViewMatrix() * pos;
}

geometry::DVector3D RCamera::ViewToWorld(const geometry::DVector3D &pos) const {
    return GetUnviewMatrix() * pos;
}

geometry::DVector3D RCamera::ViewToProject(const geometry::DVector3D &pos) const {
    return GetProjectMatrix() * pos;
}

geometry::DVector3D RCamera::ProjectToView(const geometry::DVector3D &pos) const {
    return GetUnprojectMatrix() * pos;
}

geometry::DVector3D RCamera::WorldToProject(const geometry::DVector3D &pos) const {
    return GetProjectMatrix() * (GetViewMatrix() * pos);
}

geometry::DVector3D RCamera::ProjectToWorld(const geometry::DVector3D &pos) const {
    return GetUnviewMatrix() * (GetUnprojectMatrix() * pos);
}

void RCamera::SetForwardDirection(const geometry::DVector3D& forward) {
    std::lock_guard lg(lock_);
    this->forward_ = forward;
    ComputeViewMatrix();
}

void RCamera::SetUpDirection(const geometry::DVector3D& up) {
    std::lock_guard lg(lock_);
    this->up_ = up;
    ComputeViewMatrix();
}

geometry::DVector3D RCamera::GetForwardDirection() const {
    std::lock_guard lg(lock_);
    return forward_;
}

geometry::DVector3D RCamera::GetUpDirection() const {
    std::lock_guard lg(lock_);
    return up_;
}

void RCamera::SetViewDirection(ViewDirection dir) {
    std::lock_guard lg(lock_);
    switch (dir) {
        case Isometric: {
            DVector3D xyProj(std::cos(CBD_M_PI_4), std::sin(CBD_M_PI_4), 0.);                 // y = x
            DVector3D xyCross = DVector3D::crossProduct(DVector3D(0, 0, 1), xyProj);  // y = -x
            DQuaternion q = DQuaternion::fromAxisAndAngle(xyCross, 45);
            forward_ = q.rotatedVector(xyProj);
            up_ = DVector3D::crossProduct(forward_, xyCross);
            Reset(false);
            auto n = WorldToView(rotationCenter_);
            translation_ = geometry::DVector2D(n.x(), n.y());
        } break;
        case Front:
            forward_ = DVector3D(0, 1, 0);
            up_ = DVector3D(0, 0, 1);
            break;
        case Back:
            forward_ = DVector3D(0, -1, 0);
            up_ = DVector3D(0, 0, 1);
            break;
        case Left:
            forward_ = DVector3D(1, 0, 0);
            up_ = DVector3D(0, 0, 1);
            break;
        case Right:
            forward_ = DVector3D(-1, 0, 0);
            up_ = DVector3D(0, 0, 1);
            break;
        case Top:
            forward_ = DVector3D(0, 0, -1);
            up_ = DVector3D(0, 1, 0);
            break;
        case Bottom:
            forward_ = DVector3D(0, 0, 1);
            up_ = DVector3D(0, -1, 0);
            break;
        default: break;
    }
    ComputeViewMatrix();
}

void RCamera::FitBoundingBox(const geometry::BoundBox &bbox, double margin) {
    std::lock_guard lg(lock_);
    FVector3D corners[8];
    bbox.getCorners(corners);
    DMatrix4x4 vm = GetViewMatrix();
    for (auto &c : corners) { c = vm * DVector3D(c); }
    double xMin = corners[0].x(), xMax = corners[0].x();
    double yMin = corners[0].y(), yMax = corners[0].y();
    for (size_t i = 1; i < 8; ++i) {
        xMin = std::min(xMin, (double)corners[i].x());
        xMax = std::max(xMax, (double)corners[i].x());
        yMin = std::min(yMin, (double)corners[i].y());
        yMax = std::max(yMax, (double)corners[i].y());
    }

    double w = xMax - xMin;
    double h = yMax - yMin;
    double clipAspect = w / h;
    double viewportAspect = viewportWidth_ / viewportHeight_;
    clipHeight_ = h;
    translation_.setX((xMin + xMax) / 2);
    translation_.setY((yMin + yMax) / 2);
    if (clipAspect > viewportAspect) { clipHeight_ *= (clipAspect / viewportAspect); }
    clipHeight_ *= (1.0 + 2 * margin);
    clipAspect_ = viewportAspect;
    ComputeProjMatrix();
}

const geometry::DMatrix4x4 &RCamera::GetProjectMatrix() const {
    std::lock_guard lg(lock_);
    return projMat_;
}

const geometry::DMatrix4x4 &RCamera::GetUnprojectMatrix() const {
    std::lock_guard lg(lock_);
    return unprojMat_;
}

const geometry::DMatrix4x4 &RCamera::GetViewMatrix() const {
    std::lock_guard lg(lock_);
    return viewMat_;
}

const geometry::DMatrix4x4 &RCamera::GetUnviewMatrix() const {
    std::lock_guard lg(lock_);
    return unviewMat_;
}

geometry::DMatrix3x3 RCamera::GetViewRotationMatrix3() const {
    std::lock_guard lg(lock_);
    DVector3D right = DVector3D::crossProduct(forward_, up_).normalized();
    double m[9] = {right.x(), right.y(),     right.z(),     up_.x(),      up_.y(),
                  up_.z(),   -forward_.x(), -forward_.y(), -forward_.z()};
    return DMatrix3x3(m);
}

geometry::DMatrix4x4 RCamera::GetViewRotationMatrix4() const {
    std::lock_guard lg(lock_);
    DVector3D right = DVector3D::crossProduct(forward_, up_).normalized();
    return DMatrix4x4(right.x(), right.y(), right.z(), 0.f, up_.x(), up_.y(), up_.z(), 0.f, -forward_.x(),
                      -forward_.y(), -forward_.z(), 0.f, 0.f, 0.f, 0.f, 1.f);
}

bool RCamera::ViewportMapToWorldPlane(const geometry::DVector2D& viewport_pos,
    const geometry::DVector3D& plane_origin,
    const geometry::DVector3D& plane_normal,
    geometry::DVector3D& pos_on_plane) const {
    std::lock_guard lg(lock_);
    // 获取映射到观察坐标系下的坐标
    DVector3D view_pos = ViewportPointToFarClip(viewport_pos);
    DVector3D world_pos = ViewToWorld(view_pos);
    // 获取眼睛到view_pos视线方向
    DVector3D ray_dir = GetRayDirection(world_pos);
    double n_dot_ray_dir = DVector3D::dotProduct(plane_normal, ray_dir);
    double n_dot_fwd_dir = DVector3D::dotProduct(plane_normal, forward_);
    if (std::fabs(n_dot_ray_dir) < 1e-2 || std::fabs(n_dot_fwd_dir) < 1e-2) // 很小的角度，不返回
        return false;

    // 点到平面的距离，除以（直边/斜边比例）
    double t = -DVector3D::dotProduct(plane_normal, world_pos - plane_origin) / n_dot_ray_dir;
    pos_on_plane = world_pos + t * ray_dir;
    return true;
}

bool RCamera::ViewportMapToAxisPos(const geometry::DVector2D& mouse_pos,
    const geometry::DVector3D& axis_origin,
    const geometry::DVector3D& axis_dir,
    geometry::DVector3D& pos_on_axis) const {
    std::lock_guard lg(lock_);
    // 选取一个过axis_origin，切平行于axis_dir，并且在相机视线的。
    // 如果没有符合则返回false
    DVector3D axis_n = axis_dir.normalized();
    DVector3D a_dir = DVector3D::crossProduct(axis_n, forward_);
    if (MathUtil::FuzzyIsNull(a_dir.x()) &&
        MathUtil::FuzzyIsNull(a_dir.y()) &&
        MathUtil::FuzzyIsNull(a_dir.z())) {
        return false;
    }
    DVector3D plane_nrml = DVector3D::crossProduct(axis_n, a_dir);
    plane_nrml.normalize();

    DVector3D pos;
    if (!ViewportMapToWorldPlane(mouse_pos, axis_origin, plane_nrml, pos)) {
        return false;
    }

    pos_on_axis = axis_n * DVector3D::dotProduct(axis_n, pos);
    return true;
}

geometry::DVector3D RCamera::GetRayDirection(const geometry::DVector3D& worldPos) const {
    std::lock_guard lg(lock_);
    if (mode_ == Orthometric) {
        return forward_;
    }
    else {
        DVector3D eye = -forward_ * distance_;
        DVector3D right = DVector3D::crossProduct(forward_, up_);
        eye += right * translation_.x();
        eye += up_ * translation_.y();
        return (worldPos - eye).normalized();
    }
}

void RCamera::GetClipWidthAndHeight(double& w, double& h, const geometry::DVector3D& ref_world_point, bool per) const {
    std::lock_guard lg(lock_);
    if (mode_ == Orthometric && !per) {
        h = clipHeight_;
    }
    else { // Perspective 使用零平面
        double dis = std::abs(WorldToView(ref_world_point).z());
        double fov = MathUtil::DegreeToRadian(fov_);
        h = std::tan(fov * 0.5) * dis * 2;
    }
    w = clipAspect_ * h;
}

void RCamera::ComputeViewMatrix() {
    viewMat_.setToIdentity();
    viewMat_.lookAt(-forward_ * distance_, -forward_ * (distance_ - 1.f), up_);
    unviewMat_ = viewMat_.inverted();
}

void RCamera::ComputeProjMatrix() {
    double near, far;
    GetClipNearAndFar(near, far);
    projMat_.setToIdentity();
    if (mode_ == Orthometric) {
        double half_w = clipHeight_ * clipAspect_ * 0.5;
        double half_h = clipHeight_ * 0.5;
        projMat_.ortho(-half_w, half_w, -half_h, half_h, near, far);
    }
    else {
        projMat_.perspective(fov_, clipAspect_, near, far);
    }
    if (isZeroToOneDepth_) {  // 设置NDC深度为[0, 1]
        projMat_(2, 2) = -1.0 / (far - near);
        projMat_(2, 3) = -near / (far - near);
    }

    projMat_.translate(-translation_.x(), -translation_.y());

    unprojMat_ = projMat_.inverted();
}

void RCamera::Reset(bool need_set_view) {
    if(need_set_view)
        SetViewDirection(Isometric);
    fov_ = 45.f;

    // 预设的假值
    translation_ = DVector2D(0, 0) + DVector2D(rotationCenter_.x(), rotationCenter_.y());
    clipHeight_ = 300;
    clipAspect_ = 1.5;
    // 假设的
    viewportHeight_ = clipHeight_;
    viewportWidth_ = viewportHeight_ * clipAspect_;

    isZeroToOneDepth_ = true;

    if (mode_ == Perspective) {
        double fov = MathUtil::DegreeToRadian(fov_);
        distance_ = clipHeight_ * 0.5 / std::tan(fov * 0.5);
    }
    else {
        distance_ = Clip_Radius;
    }

    ComputeViewMatrix();
    ComputeProjMatrix();
}

void RCamera::UpdateTranslationForRotate(const geometry::DQuaternion& q) {
    if (MathUtil::FuzzyIsNull(rotationCenter_.x()) &&
        MathUtil::FuzzyIsNull(rotationCenter_.y()) &&
        MathUtil::FuzzyIsNull(rotationCenter_.z())) {
        return;
    }

    DVector3D centerAfterRotate = q.rotatedVector(rotationCenter_);
    // 依赖旋转后新的模型视图矩阵
    DVector3D oldCenterOnView = WorldToView(rotationCenter_);
    DVector3D newCenterOnView = WorldToView(centerAfterRotate);
    DVector3D offset = newCenterOnView - oldCenterOnView;
    translation_[0] -= offset.x();
    translation_[1] -= offset.y();
    if (mode_ == Perspective) {
        distance_ -= offset.z();
    }

    ComputeProjMatrix();
    ComputeViewMatrix();
}

void RCamera::GetClipNearAndFar(double& near, double& far) const {
    std::lock_guard lg(lock_);
    if (mode_ == Perspective) {
        // 近/远平面始终在-Clip_Radius~Clip_Radius子集范围内
        double near_pos = distance_ - 1;
        near_pos = std::max(-Clip_Radius, std::min(Clip_Radius, near_pos));
        double far_pos = -Clip_Radius;
        // 使用更合理的近平面值，避免深度精度问题
        // 对于靠近的模型，near平面应该更大以减少深度竞争
        double min_near = distance_ * 0.01;  // 近平面至少为距离的1%
        min_near = std::max(min_near, 0.1);  // 至少为0.1，避免过小
        near = std::max(distance_ - near_pos, min_near);
        // 存在far比near小的情况，当为该条件时，另far=near
        far = std::max(distance_ - far_pos, near);
        // 限制far/near的比值，减少深度精度损失
        // 理想的比值应该在1000:1以内，超过会导致深度精度问题
        if (far / near > 1000.0) {
            far = near * 1000.0;
        }
    }
    else {
        // 正交模式下，near不应该为0，这会导致深度精度问题
        near = 0.1;
        far = 2 * Clip_Radius;
    }
}

bool RCamera::CalculateZoomForOrtho(double factor, double& new_height) {
    // 当鼠标消息因阻塞，可能累计计算出极小值，负值，很大的值，拒绝处理
    if (factor < 0.1 || factor > 1.9 || MathUtil::FuzzyIsEqual(factor, 1.0)) {
        return false;
    }

    new_height = clipHeight_ / factor;
    if (factor > 1) {
        if (clipHeight_ < Clip_Min_Height) {
            return false;
        }
    }
    //缩小
    else {
        if (clipHeight_ > Clip_Max_Height) {
            return false;
        }
    }
    return true;
}

bool RCamera::CalculateZoomForPersp(double factor, double& new_distance) {
    // 当鼠标消息因阻塞，可能累计计算出极小值，负值，很大的值，拒绝处理
    if (factor < 0.1 || factor > 1.9 || MathUtil::FuzzyIsEqual(factor, 1.0)) {
        return false;
    }
    DVector3D oldCenterOnView = WorldToView(rotationCenter_);
    //下面两句代码把zd换算到当前中心点，然后计算得到准确的factor，然后再得到当前的新distance
    float offset_z = -(oldCenterOnView.z());
    double depth = offset_z + Perspective_Zoom_Max_Negative_Depth;
    // 在限定范围内，重调比例
    if (std::abs(offset_z) < Perspective_Zoom_Max_Negative_Depth) {
        constexpr double min_fac2 = 0.2; // 最小二级倍率
        constexpr double tol_range_dis = Perspective_Zoom_Max_Negative_Depth * 2;
        double cur_rangle_dis = offset_z + Perspective_Zoom_Max_Negative_Depth;
        double cur_range_fac = cur_rangle_dis / tol_range_dis;
        // 二次曲率
        double fac2 = (1.f - min_fac2) * cur_range_fac * cur_range_fac + min_fac2;
        factor = (factor - 1.0) * fac2 + 1.0;
    }
    auto last_d = offset_z;
    if (depth <= 0) { // 跨越最大负向深度时
        new_distance = distance_ * factor;
    }
    else {
        new_distance = depth / factor - Perspective_Zoom_Max_Negative_Depth;
    }
    if (factor > 1) {
        if (new_distance < -Perspective_Zoom_Max_Negative_Depth) {
            return false;
        }
    }
    else {
        if (new_distance > 2 * Clip_Radius) {
            return false;
        }
    }
    float del = new_distance  - last_d;
    new_distance = distance_ + del;
    return true;
}

void RCamera::CopyFrom(const RCamera& other) {
    mode_ = other.mode_;
    forward_ = other.forward_;
    up_ = other.up_;
    distance_ = other.distance_;
    fov_ = other.fov_;

    rotationCenter_ = other.rotationCenter_;
    viewMat_ = other.viewMat_;
    unviewMat_ = other.unviewMat_;

    projMat_ = other.projMat_;
    unprojMat_ = other.unprojMat_;

    translation_ = other.translation_;
    clipHeight_ = other.clipHeight_;
    clipAspect_ = other.clipAspect_;

    viewportWidth_ = other.viewportWidth_;
    viewportHeight_ = other.viewportHeight_;

    isZeroToOneDepth_ = other.isZeroToOneDepth_;
}
}  // namespace render

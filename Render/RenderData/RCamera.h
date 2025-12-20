/**
 *@author		: Ye Changyu
 *@date			: 2023-10-26
 *@description	: 相机类，生成视图矩阵和投影矩阵，提供世界/视图/投影坐标系之间的坐标转换，
 *                以及视口到投影平面的坐标转换
 */

#ifndef _RCamera_H_
#define _RCamera_H_

#include <mutex>
#include "geometry/DVector2D.h"
#include "geometry/DVector3D.h"
#include "geometry/DMatrix4x4.h"
#include "geometry/BoundBox.h"

// 透视模式下缩放的最大负向深度
#define Perspective_Zoom_Max_Negative_Depth 200

namespace render {
class RCamera {
public:
    enum ProjectMode { Orthometric, Perspective };
    enum  ViewDirection { Isometric, Front, Back, Left, Right, Top, Bottom };
    // enum EyeType { LeftEye, RightEye };  // 当前仅实现RightEye
    enum Plane { XOY, YOZ, ZOX };

    RCamera();
    ~RCamera();
    RCamera(const RCamera& other);
    RCamera& operator=(const RCamera& other);

    void SetProjectMode(ProjectMode mode);
    ProjectMode GetProjectMode() const;

    // 设置NDC空间深度范围是否为[0,1]范围
    void SetZeroToOneDepth(bool is021);
    bool IsZeroToOneDepth() const;

    void Pitch(double angle, bool base_rotation_center = false);
    void Yaw(double angle, bool base_rotation_center = false);
    void Roll(double angle, bool base_rotation_center = false);

    void RollOnWorld(double angle, bool base_rotation_center = false);

    void Pan(const geometry::DVector2D &offset);
    void Zoom(double factor);
    void ZoomAtPoint(const geometry::DVector2D& mousePos, double factor);

    void SetViewDirection(ViewDirection dir);
    // 适应给定边框（模型空间坐标系），margin为边界留白比值 ResizeViewport一定要放该函数前面
    void FitBoundingBox(const geometry::BoundBox &bbox, double margin = 0.1);

    const geometry::DMatrix4x4 &GetProjectMatrix() const;
    const geometry::DMatrix4x4 &GetUnprojectMatrix() const;

    const geometry::DMatrix4x4 &GetViewMatrix() const;
    const geometry::DMatrix4x4 &GetUnviewMatrix() const;

    geometry::DMatrix3x3 GetViewRotationMatrix3() const;
    geometry::DMatrix4x4 GetViewRotationMatrix4() const;
    //获取p上的射线和三角形的交点
    bool GetRayInterTri(int px, int py, geometry::FVector3D tri[3], geometry::FVector3D& pos_at_model) const;
    void ResizeViewport(int w, int h);
    int GetViewPortWidth() const;
    int GetViewPortHeight() const;
    void CalcuFullVisibility(geometry::BoundBox box);
    // 返回视口点击点在裁切面的坐标
    // 指定参考点（世界坐标），如果不为nullptr，则以改点所在的裁切体平面进行计算；如果为nullptr，使用世界坐标原点
    geometry::DVector3D ViewportPointToClip(const geometry::DVector2D &pos,
        const geometry::DVector3D& ref_world_point = geometry::DVector3D(0, 0, 0)) const;
    geometry::DVector3D ViewportPointToNearClip(const geometry::DVector2D& pos) const;
    geometry::DVector3D ViewportPointToFarClip(const geometry::DVector2D& pos) const;

    geometry::DVector3D WorldToView(const geometry::DVector3D&pos) const;
    geometry::DVector3D ViewToWorld(const geometry::DVector3D&pos) const;

    geometry::DVector3D ViewToProject(const geometry::DVector3D&pos) const;
    geometry::DVector3D ProjectToView(const geometry::DVector3D&pos) const;

    geometry::DVector3D WorldToProject(const geometry::DVector3D&pos) const;
    geometry::DVector3D ProjectToWorld(const geometry::DVector3D&pos) const;

    geometry::DVector3D GetForwardDirection() const;
    geometry::DVector3D GetUpDirection() const;
    geometry::DVector3D GetRightDirection() const;
    double              GetDistance() const;

    void                SetRotationCenter(const geometry::DVector3D& center);
    geometry::DVector3D GetRotationCenter() const;
    void                ResetRotationCenter();

    void                GetXYZDistance(float& x, float& y , float& z) const;
    void                SetXYZDistance(float x, float y , float z);

    void                SetForwardDirection(const geometry::DVector3D& forward);
    void                SetUpDirection(const geometry::DVector3D& up);

    // 获取视口点映射到指定世界坐标平面上的点，无交返回false
    bool               ViewportMapToWorldPlane(const geometry::DVector2D& mouse_pos,
                                               const geometry::DVector3D& plane_origin,
                                               const geometry::DVector3D& plane_normal,
                                               geometry::DVector3D& pos_on_plane) const;
    // 获取视口点映射到指定世界坐标轴的位置，失败返回false
    bool               ViewportMapToAxisPos(const geometry::DVector2D& mouse_pos,
                                            const geometry::DVector3D& axis_origin,
                                            const geometry::DVector3D& axis_dir,
                                            geometry::DVector3D& pos_on_axis) const;
    // 正交返回forward；透视返回眼睛实际位置到指定worldPos的向量
    geometry::DVector3D GetRayDirection(const geometry::DVector3D& worldPos) const;

    // 通过给定点（世界坐标）的裁切面宽高
    void GetClipWidthAndHeight(double& w, double& h,
        const geometry::DVector3D&ref_world_point = geometry::DVector3D(0, 0, 0), bool per = false) const;

protected:
    void ComputeViewMatrix();
    void ComputeProjMatrix();

    void Reset(bool need_set_view = true);

    void UpdateTranslationForRotate(const geometry::DQuaternion& q);
    void GetClipNearAndFar(double& near, double& far) const;

    bool CalculateZoomForOrtho(double factor, double& new_height);
    bool CalculateZoomForPersp(double factor, double& new_distance);

    void CopyFrom(const RCamera& other);

protected:
    ProjectMode mode_;
    geometry::DVector3D forward_;
    geometry::DVector3D up_;
    double distance_;
    double fov_;
    //旋转中心
    geometry::DVector3D rotationCenter_;
    geometry::DMatrix4x4 viewMat_;
    geometry::DMatrix4x4 unviewMat_;

    geometry::DMatrix4x4 projMat_;
    geometry::DMatrix4x4 unprojMat_;

    geometry::DVector2D translation_;
    double clipHeight_;
    double clipAspect_;  // (clip width) / (clip height)

    double viewportWidth_;
    double viewportHeight_;

    bool isZeroToOneDepth_;
    mutable std::recursive_mutex lock_;
};
}  // namespace render

#endif  // _RCamera_H_

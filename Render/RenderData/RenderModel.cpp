
#include "RenderModel.h"
#include <fstream>
namespace render {

std::atomic<int> g_unique_id = 0;

RenderModel::RenderModel() : model_id_(g_unique_id.fetch_add(1)){

}

void RenderModel::ProceseModelAfterLoad() {
    vertexElement.CalculateModelSize(m_modelMinPos, m_modelMaxPos);
    m_modelCenterPos = (m_modelMaxPos + m_modelMinPos) / 2;
    m_modelSize = m_modelMaxPos - m_modelMinPos;

    PlaceModelCenterToCenter(false);

    // m_vertexElements.proceseModelAfterLoad();
    // m_vertexIndexCount = m_vertexElements.getVertexIndexCount(VertexElement::IDX_NONE_FLAG);
    // calculateModelSize();

    // recaculateZPosUpdataTrans();
    // 保存模型原始大小
    m_translate.setZ(m_modelSize.z() / 2);
    // 清楚去重用的map
    // clearLoadMap();
}

void RenderModel::Triangle(geometry::FVector3D &v1, geometry::FVector3D &v2, geometry::FVector3D &v3) { vertexElement.triangle(v1, v2, v3); }

void RenderModel::GatherMatrix4x4(geometry::DMatrix4x4 &mat) {
    auto translate = m_translate2 + m_translate;
    mat.translate(translate.x(), translate.y(), translate.z() + m_zDistanceFromPlatform);

    mat *= m_rotateMatrix;

    if (m_oldScale != m_scale) {
        geometry::DMatrix4x4 scaleMat;
        geometry::FVector3D scaleRatio(m_scale.x() / m_oldScale.x(), m_scale.y() / m_oldScale.y(),
                             m_scale.z() / m_oldScale.z());
        scaleMat.scale(scaleRatio.x(), scaleRatio.y(), scaleRatio.z());
        mat *= scaleMat;
    }
}

void RenderModel::ApplyMat(geometry::FMatrix4x4& mat) {
    auto& v_vec = this->vertexElement.vertices;
    for (int i = 0; i < v_vec.size(); i++) {
        v_vec[i] = mat * v_vec[i];
    }
}

void RenderModel::PlaceModelCenterToCenter(bool keepPosition) {
    if (m_modelCenterPos.x() != 0.0 || m_modelCenterPos.y() != 0.0 || m_modelCenterPos.z() != 0.0) {
        // 将模型的中心平移到原点
        vertexElement.offset(geometry::FVector3D(-m_modelCenterPos));
    }
    m_modelCenterPos = geometry::FVector3D(0.0, 0.0, 0.0);
    if (keepPosition) m_translate += m_modelCenterPos;
    m_modelMinPos = -m_modelSize / 2;
    m_modelMaxPos = m_modelSize / 2;
}
}


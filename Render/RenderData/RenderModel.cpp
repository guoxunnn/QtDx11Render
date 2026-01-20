
#include "RenderModel.h"
#include <fstream>

std::shared_ptr<RenderModel> RenderModel::LoadStlFile(const char* filePath, bool is_need_place_on_platform_flag) {
    // LOGI << "start load file:" << filePath;
    std::shared_ptr<RenderModel> p_model = std::make_shared<RenderModel>();
    std::ifstream fin(filePath, std::ifstream::in | std::ifstream::binary);

    int triangleCount = 0;

    if (fin.bad()) return false;
    fin.seekg(80, std::ios::beg);
    int faceCount = 0;
    fin.read((char*)&faceCount, 4);
    int currentPos = fin.tellg();
    if (fin.gcount() <= 0) return false;

    fin.seekg(0, fin.end);
    int fileSize = fin.tellg();
    fin.seekg(currentPos, fin.beg);

    float v[13];
    int faceByteSize = 12 * 4 + 2;

    for (int i = 0; i < faceCount; i++) {
        fin.read((char*)(v), faceByteSize);
        geometry::FVector3D point1(v[3], v[4], v[5]), point2(v[6], v[7], v[8]), point3(v[9], v[10], v[11]);
        p_model->Triangle(point1, point2, point3);
        triangleCount++;
        // if (i <= 20) {
        //     LOGI << " " << point1.x() << " " << point1.y() << " " << point1.z() << " ";
        //     LOGI << " " << point2.x() << " " << point2.y() << " " << point2.z();
        //     LOGI << " " << point3.x() << " " << point3.y() << " " << point3.z();
        // }
    }
    if(is_need_place_on_platform_flag)
        p_model->ProceseModelAfterLoad();
    return p_model;
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


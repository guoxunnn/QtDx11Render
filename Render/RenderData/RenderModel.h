
#ifndef RENDERMODEL_H
#define RENDERMODEL_H

#include "geometry/FVector3D.h"
#include "geometry/FMatrix4x4.h"
#include "geometry/DMatrix4x4.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
namespace render {

class VertexElement {
public:
    void SetModifyFlagFalse() {
        this->is_modify_flag_ = false;
    }
    bool IsModify() const {return is_modify_flag_;}

    void CalculateModelSize(geometry::FVector3D &minPos, geometry::FVector3D &maxPos) {
        float inf = 1e9;
        float minx = inf, miny = inf, minz = inf, maxx = -inf, maxy = -inf, maxz = -inf;
        for (auto point : vertices) {
            float x = point.x();
            float y = point.y();
            float z = point.z();
            minx = minx > x ? x : minx;
            miny = miny > y ? y : miny;
            minz = minz > z ? z : minz;
            maxx = maxx < x ? x : maxx;
            maxy = maxy < y ? y : maxy;
            maxz = maxz < z ? z : maxz;
        }
        minPos = geometry::FVector3D(minx, miny, minz);
        maxPos = geometry::FVector3D(maxx, maxy, maxz);
    }

    void triangle(geometry::FVector3D &v1, geometry::FVector3D &v2, geometry::FVector3D &v3) {
        insertVertex(v1);
        insertVertex(v2);
        insertVertex(v3);
    }
    void insertVertex(geometry::FVector3D &v) {
        int lastIndex = vertices.size();
        vertices.push_back(v);
        std::string str;
        str.resize(12);
        memcpy((void *)str.data(), &v, 12);
        int index = 0;
        std::unordered_map<std::string, int>::iterator it = m_vertexHash.find(str);

        if (it == m_vertexHash.end()) {
            m_vertexHash.insert(std::pair<std::string, int>(str, lastIndex));
            index = lastIndex;
        } else {
            vertices.pop_back();
            index = it->second;
        }
        m_insertIndexArray[m_insertIndex] = index;
        if (m_insertIndex == 2) {
            indices.push_back(m_insertIndexArray[0]);
            indices.push_back(m_insertIndexArray[1]);
            indices.push_back(m_insertIndexArray[2]);
        }
        m_insertIndex = (++m_insertIndex) % 3;
    }

    void offset(geometry::FVector3D offset) {
        for (int i = 0; i < vertices.size(); ++i) vertices[i] += offset;
    }

public:
    std::vector<int> indices;
    std::vector<geometry::FVector3D> vertices;

private:
    std::unordered_map<std::string, int> m_vertexHash;
    int m_insertIndexArray[3] = {0};
    int m_insertIndex = 0;
    bool is_modify_flag_ = true;


};

class RenderModel {
public:
    RenderModel();
    void ProceseModelAfterLoad();
    void Triangle(geometry::FVector3D &v1, geometry::FVector3D &v2, geometry::FVector3D &v3);

    void GatherMatrix4x4(geometry::DMatrix4x4 &mat);
    void ApplyMat(geometry::FMatrix4x4& mat);
    void PlaceModelCenterToCenter(bool keepPosition = false);
    int64_t ID() const {return this->model_id_;}

public:
    geometry::FVector3D m_scale = geometry::FVector3D(1, 1, 1);
    geometry::FVector3D m_oldScale = geometry::FVector3D(1, 1, 1);

    geometry::FVector3D m_translate = geometry::FVector3D(0, 0, 0);
    geometry::FVector3D m_translate2 = geometry::FVector3D(0, 0, 0);

    geometry::FVector3D m_rotateXYZ = geometry::FVector3D(0, 0, 0);
    geometry::FVector3D m_modelMinPos;
    geometry::FVector3D m_modelMaxPos;
    geometry::FVector3D m_modelCenterPos;
    geometry::FVector3D m_modelSize;
    VertexElement vertexElement;

    geometry::DMatrix4x4 m_rotateMatrix;

    // 仅世界缩放时使用的尺寸
    geometry::FVector3D m_scale_world;
    // 模型z轴距离平台的距离
    float m_zDistanceFromPlatform = 0.0f;

private:
    int64_t model_id_ = 0;

};
}

#endif // RENDERMODEL_H

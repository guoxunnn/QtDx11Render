#include "ModelRenderManager.h"
#include "../../RenderData/RenderContext.h"
#include <atomic>
#include <fstream>

namespace render {

ModelRenderManager::ModelRenderManager() {
    LoadStlFile("./Resources/TestStl/Chicken.stl", true);
}

void ModelRenderManager::BuildModelGpuBuffer(std::shared_ptr<RenderContext> context) {
    for(int i = 0; i < test_render_model_vec.size(); i++) {
        CreateModelBufferFromRenderModelData(test_render_model_vec[i].get(), context);
    }
}

std::shared_ptr<ModelBuffer> ModelRenderManager::CreateModelBufferFromRenderModelData(RenderModel* p_model, std::shared_ptr<RenderContext> context) {
    std::shared_ptr<ModelBuffer> res_model = nullptr;
    if(test_model_bufer_map_.find(p_model->ID()) != test_model_bufer_map_.end()) {
        res_model = test_model_bufer_map_[p_model->ID()];
    }else {
        res_model = std::make_shared<ModelBuffer>();
    }
    auto cur_id = p_model->ID();
    if(p_model->vertexElement.IsModify()) {
        return res_model;
    }
    const auto& vertices = p_model->vertexElement.vertices;
    const auto& indices = p_model->vertexElement.indices;
    auto buffer = std::make_shared<BlockBuffer>(cur_id, nullptr);
    auto engine_interface = context->GetRenderEngineInterface();
    int vertexBufferSize = vertices.size() * sizeof(geometry::FVector3D) / sizeof(float);
    engine_interface->CreateGpuVertexBuffer((float*)vertices.data(), vertexBufferSize, buffer->vertex_handle_);
    engine_interface->CreateGpuIndexBuffer((uint32_t*)indices.data(), indices.size(), buffer->index_handle_);

    buffer->index_size_ = indices.size();
    buffer->stride_ = sizeof(geometry::FVector3D);
    res_model->block_buffers_.push_back(buffer);
    res_model->model_id_ = cur_id;
    test_model_bufer_map_[cur_id] = res_model;
    return res_model;
}

std::shared_ptr<RenderModel> ModelRenderManager::LoadStlFile(const char* filePath, bool is_need_place_on_platform_flag) {
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

}



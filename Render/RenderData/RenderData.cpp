#include "RenderData.h"

#include "../../manager/RenderEngineInterface.h"
#include "../../model/STLMeshLoad.h"
#include "../../utils/ConfigHelper.h"
#include "../MeasureInfo.h"
#include "../ModelRenderManager.h"
namespace render {
void CutFaceData::SetCurNormalAndPnt(const geometry::FVector3D& p, const geometry::FVector3D& n) {
    std::unique_lock<std::mutex> auto_lock(mutex_);
    updata_flag = true;
    this->normal_ = n;
    this->point_ = p;
}

std::shared_ptr<BlockBuffer> CutFaceData::GetBlockBuffer() const {
    return this->user_assgin_cut_face_;
}

void CutFaceData::BuildData(ModelRenderManager* manager) {
    std::unique_lock<std::mutex> auto_lock(mutex_);
    if (!updata_flag) {
        return;
    }
    manager->DeleteBlockBuffer(user_assgin_cut_face_);
    user_assgin_cut_face_ = std::make_shared<BlockBuffer>(-1, nullptr);
    user_assgin_cut_face_->vertex_size = 6;
    auto four_point = MeasureInfo::GetPointAndNormalCreateFourPoint(point_, normal_, 100000);
    std::vector<geometry::FVector3D> side;
    auto offset = normal_.normalized() * 0;
    side.push_back(four_point[0] + offset);
    side.push_back(four_point[1] + offset);
    side.push_back(four_point[2] + offset);

    side.push_back(four_point[2] + offset);
    side.push_back(four_point[3] + offset);
    side.push_back(four_point[0] + offset);
    manager->render_engine_interface_->CreateGpuVertexBuffer((float*)side.data(), side.size() * 3,
                                                             user_assgin_cut_face_->vertex_handle_);
    std::vector<unsigned int> index_vec = {0, 1, 2, 3, 4, 5};
    manager->render_engine_interface_->CreateGpuIndexBuffer((uint32_t*)index_vec.data(), index_vec.size(),
        user_assgin_cut_face_->index_handle_);
    user_assgin_cut_face_->index_size = 6;
}

void DebugRenderData::Build(ModelRenderManager* manager, std::shared_ptr<algo::AlgoObjectInterface> algoObject_) {
    static bool test_flag_ = true;
     if(0){
         test_flag_ = false;
        algoObject_->ClearDebugData();
        {

            std::vector<geometry::FVector3D> vec;
            algo::RenderDataInfo test;
            for (int i = 0; i < 1000; i++) {
                vec.push_back(geometry::FVector3D(i, 0, 30));
                vec.push_back(geometry::FVector3D(i, 10, 30));
                vec.push_back(geometry::FVector3D(i, 0, 40));
            }

            algo::AlgoVector<algo::RenderDataInfo> data;
            test.data.resize(vec.size() * 3);
            memcpy(test.data.data(), vec.data(), vec.size() * sizeof(geometry::FVector3D));
            test.color = algo::AlgoColor32(255, 0, 0, 255);
            test.type = algo::DebugRenderType::Debug_Type_Tri;
            data.push_back(test);
            algoObject_->AppendDebugData(data);
        }

        if(1){
            algo::AlgoVector<algo::RenderDataInfo> data;
            for (int i = 0; i < 100; i++) {
                algo::RenderDataInfo test;
                std::vector<geometry::FVector3D> vec;
                vec.push_back(geometry::FVector3D(i, 0, 10));
                vec.push_back(geometry::FVector3D(1, 0, 0));
                vec.push_back(geometry::FVector3D(i, 10, 10));
                vec.push_back(geometry::FVector3D(0, 1, 0));
                vec.push_back(geometry::FVector3D(i, 0, 20));
                vec.push_back(geometry::FVector3D(0, 0, 1));
                test.data.resize(vec.size() * 3);
                memcpy(test.data.data(), vec.data(), vec.size() * sizeof(geometry::FVector3D));
                test.is_data_have_color_flag_ = true;
                test.type = algo::DebugRenderType::Debug_Type_Tri;
                data.push_back(test);
            }
            algoObject_->AppendDebugData(data);
        }
     }
    algo::AlgoVector<algo::RenderDataInfo> debug_data;
    bool res = algoObject_->GetDebugData(debug_data);
    if (!res) {
        return;
    }
    this->debug_buf_vec_.resize(debug_data.size());
    for (int i = 0; i < debug_data.size(); i++) {
        auto& d = debug_data[i];
        if (d.is_update_flag_ && d.data.size()) {
            manager->render_engine_interface_->CreateGpuVertexBuffer((float*)d.data.data(), d.data.size(),
                debug_buf_vec_[i].debug_buf_.vertex_handle_);
            debug_buf_vec_[i].debug_buf_.vertex_size = d.data.size() / 3;
            if (d.is_data_have_color_flag_) {
                debug_buf_vec_[i].debug_buf_.vertex_size /= 2;
            }
            debug_buf_vec_[i].color = d.color;
            debug_buf_vec_[i].type = d.type;
            debug_buf_vec_[i].is_data_have_color_flag_ = d.is_data_have_color_flag_;
        }
    }
}

void CutSectionData::SectionData::destory(ModelRenderManager* manager) {
    if (line_.vertex_handle_) {
        manager->DeleteLineBlock(line_);
    }

    // 先删除以前的
    for (int i = 0; i < paths_.size(); i++) {
        manager->DeleteLineBlock(paths_[i]);
    }
    paths_.clear();
    if (tri_buf_) {
        manager->DeleteBlockBuffer(tri_buf_);
        tri_buf_ = nullptr;
    }
}

void CutSectionData::SectionData::matTrans2Ori(std::vector<geometry::FVector3D>& vec, geometry::DMatrix4x4 matrix) {
    bool flag = false;
    auto inv = matrix.inverted(&flag);
    if (!flag) {
        return;
    }
    for (int i = 0; i < vec.size(); i++) {
        vec[i] = inv * DVector3D(vec[i]);
    }
}

void CutSectionData::CreateRenderBuf(ModelRenderManager* manager, const algo::ModelAbstract* belong_model,
                                     ModelBuffer* buf) {
    bottom_.createSection(manager, belong_model, buf, algo::SectionAbstract::Location::Bottom);
    top_.createSection(manager, belong_model, buf, algo::SectionAbstract::Location::Top);
    left_.createSection(manager, belong_model, buf, algo::SectionAbstract::Location::Left);
    right_.createSection(manager, belong_model, buf, algo::SectionAbstract::Location::Right);
    back_.createSection(manager, belong_model, buf, algo::SectionAbstract::Location::Back);
    front_.createSection(manager, belong_model, buf, algo::SectionAbstract::Location::Fornt);
}

void CutSectionData::Destory(ModelRenderManager* manager) {
    bottom_.destory(manager);
    top_.destory(manager);
    left_.destory(manager);
    right_.destory(manager);
    back_.destory(manager);
    front_.destory(manager);
}

void CutSectionData::SectionData::createSection(ModelRenderManager* manager, const algo::ModelAbstract* belong_model,
                                                ModelBuffer* buf, algo::SectionAbstract::Location loc) {
    loc_ = loc;
    algo::SectionAbstract* sec = belong_model->GetSection(loc);
    std::vector<geometry::FVector3D> lineVec;
    bool is_modified = sec->IsModified();
    // int lineCount = sec->LineCount();
    // framework::XVector<float> data;
    // sec->LineData(data);
    // framework::XVector<framework::XVector<float>> paths_;
    // sec->PathData(paths_);
    framework::XVector<float> gray_data;
    framework::XVector<float> line_data;
    framework::XVector<framework::XVector<float>> path_data;
    sec->GetData(line_data, path_data, gray_data);

    if (!is_modified || line_data.size() == 0) {
        if (line_data.size() == 0) {
            destory(manager);
        }
        // 条件满足可能是某些部分还没构建好
        if ((line_data.size() > 0 && (!line_.vertex_handle_ || line_.vertex_size == 0)) ||
            (paths_.size() == 0 && path_data.size() > 0)
            /* || (!tri_buf_ && tri_data.index_size > 0)*/) {
        } else {
            return;
        }
    }
    auto model_mat = buf->matrix;
    // destory(manager);
    // 线
    {
        // 一个线段2个顶点

        lineVec.resize(line_data.size() / 3);
        // 因为一个顶点3个float
        if (lineVec.size()) {
            memcpy(lineVec.data(), line_data.data(), line_data.size() * sizeof(float));
            auto vvvv2 = lineVec;
            matTrans2Ori(lineVec, model_mat);
            destory(manager);
            manager->render_engine_interface_->CreateGpuVertexBuffer((float*)lineVec.data(), lineVec.size() * 3,
                                                                     line_.vertex_handle_);
            line_.vertex_size = lineVec.size();
        }

        /* line_.vertex_size = sec->LineCount() * 2;
         lineVec.resize(line_.vertex_size);
         //因为一个顶点3个float
         if (line_.vertex_size) {
             auto getD = sec->LineData();
             if (getD) {
                 memcpy(lineVec.data(), getD, lineVec.size() * sizeof(geometry::FVector3D));
                 matTrans2Ori(lineVec, model_mat);
                 destory(manager);
                 manager->CreateGpuVertexBuffer((float*)lineVec.data(), line_.vertex_size * 3,
         line_.vertex_handle_);
             }
         }*/
    }
    // 路径
    {
        size_t paths_count = path_data.size();
        if (paths_count > 0) {
            paths_.resize(paths_count);
            for (int i = 0; i < paths_count; i++) {
                VertexBlockBuffer& l = paths_[i];
                auto& path = path_data[i];
                // 首尾相连
                l.vertex_size = path.size() / 3 + 1;
                if (path.size() > 0) {
                    std::vector<geometry::FVector3D> vec;
                    vec.resize(path.size() / 3);
                    if (path.size() > 0) {
                        memcpy(vec.data(), path.data(), path.size() * sizeof(float));
                        vec.push_back(vec[0]);
                        matTrans2Ori(vec, model_mat);
                        manager->render_engine_interface_->CreateGpuVertexBuffer((const float*)vec.data(),
                                                                                 vec.size() * 3, l.vertex_handle_);
                    }
                }
            }
        }

        /*
        uint32_t paths_count = sec->PathsCount();
        paths_.resize(paths_count);

        for(int i = 0; i < paths_count; i++) {
            VertexBlockBuffer& l = paths_[i];
            //首尾相连
            l.vertex_size =  sec->PathCount(i) + 1;
            if(l.vertex_size > 1) {
                std::vector<geometry::FVector3D> vec;
                vec.resize(l.vertex_size - 1);
                auto path_data = sec->PathData(i);
                if (path_data) {
                    memcpy(vec.data(), sec->PathData(i), (l.vertex_size - 1) * 3 * sizeof(float));
                    vec.push_back(vec[0]);
                    matTrans2Ori(vec, model_mat);
                    manager->CreateGpuVertexBuffer((const float*)vec.data(), l.vertex_size * 3,
        l.vertex_handle_);
                }

            }
        }*/
    }
    // 三角形
    {
        /* std::vector<geometry::FVector3D> triVec(tri_data.index_size);
         memcpy(triVec.data(), tri_data.vertexes, tri_data.index_size * sizeof(geometry::FVector3D));
         matTrans2Ori(triVec, model_mat);
         algo::TriangleData tri2 = tri_data;
         tri2.vertexes = (const float*)triVec.data();
         tri_buf_ = manager->CreateElementBuffer(tri2);*/
        /*algo::TriangleData tri = sec->Triangles();
        std::vector<geometry::FVector3D> triVec(tri.index_size);
        memcpy(triVec.data(), tri.vertexes, tri.index_size * sizeof(geometry::FVector3D));
        matTrans2Ori(triVec, model_mat);
        algo::TriangleData tri2 = tri;
        tri2.vertexes = (const float*)triVec.data();
        tri_buf_ = manager->CreateElementBuffer(tri2);*/
    }
}

void SlicerInnerOutLayer::OnSliceUpdataEvent(topic::SliceNotify* msg, void* user_data) {
    AlgoRenderImpl* processor = (AlgoRenderImpl*)user_data;
    std::shared_ptr<topic::SliceNotify> ptr = std::make_shared<topic::SliceNotify>();
    *ptr = *msg;
    std::unique_lock<std::mutex> auto_lock(mutex_);
    path_slice_map_[msg->index + 1] = (algo::SliceResult*)msg->slice_data;
}

void SlicerInnerOutLayer::Clear(ModelRenderManager* manager) {
    std::unique_lock<std::mutex> auto_lock(mutex_);
    path_slice_map_.clear();
    for (int i = 0; i < top_slice_path_.paths_.size(); i++) {
        manager->DeleteLineBlock(top_slice_path_.paths_[i].block_);
    }

    for (int i = 0; i < bottom_slice_path_.paths_.size(); i++) {
        manager->DeleteLineBlock(bottom_slice_path_.paths_[i].block_);
    }
    top_slice_path_.paths_.clear();
    bottom_slice_path_.paths_.clear();
    top_slice_path_.layer_index_ = -1;
    bottom_slice_path_.layer_index_ = -1;
    top_slice_path_.z_height = -1000;
    bottom_slice_path_.z_height = -1000;
    top_slice_path_.is_updata_flag_ = false;
    bottom_slice_path_.is_updata_flag_ = false;
}

void SlicerInnerOutLayer::SetAllOffset(float x, float y, float z) {
    std::unique_lock<std::mutex> auto_lock(mutex_);
    all_offset_ = geometry::FVector3D(x, y, z);
}

void SlicerInnerOutLayer::SetSliceData(const algo::SliceResult::SliceLayerData& dat, bool is_top) {
    std::unique_lock<std::mutex> auto_lock(mutex_);
    auto paths_copy = [](const algo::AlgoVector<algo::SliceResult::SliceLayerData::LayerData>& orig
        , algo::AlgoVector<algo::SliceResult::SliceLayerData::LayerData>& target) {

        target.clear();
        target.resize(orig.size());
        for(int i = 0; i < orig.size(); ++i) {
            auto& target_item = target[i];
            auto& orig_item = orig[i];
            target_item.is_ccw_ = orig_item.is_ccw_;
            target_item.path.resize(orig_item.path.size());
            memcpy(target_item.path.data(), orig_item.path.data(), orig_item.path.size() * sizeof(float));
        }
    };
    auto data_copy = [paths_copy](const algo::SliceResult::SliceLayerData& orig, algo::SliceResult::SliceLayerData& target) {
        paths_copy(orig.paths, target.paths);
        paths_copy(orig.support_paths, target.support_paths);
        paths_copy(orig.support_raft_paths, target.support_raft_paths);
        paths_copy(orig.fill_inner_paths, target.fill_inner_paths);
    };
    if(is_top) {
        data_copy(dat, top_path_data_);
        top_slice_path_.is_updata_flag_ = true;
    }else {
        data_copy(dat, bottom_path_data_);
        bottom_slice_path_.is_updata_flag_ = true;
    }
}

void SlicerInnerOutLayer::SetCurSliceResult(int layer, float v, bool is_top) {
    std::unique_lock<std::mutex> auto_lock(mutex_);
    if (is_top) {
        top_slice_path_.layer_index_ = layer;
        top_slice_path_.is_updata_flag_ = true;
        top_slice_path_.z_height = v;
    } else {
        bottom_slice_path_.layer_index_ = layer;
        bottom_slice_path_.is_updata_flag_ = true;
        bottom_slice_path_.z_height = v;
    }
}

void SlicerInnerOutLayer::Build(ModelRenderManager* manager) {
    std::unique_lock<std::mutex> auto_lock(mutex_);
    auto create_path_sub = [this, manager](SlicerLayerPath& path, algo::SliceResult::SliceLayerData& data) {
        for (int i = 0; i < data.paths.size(); i++) {
            auto cur_path = data.paths[i];
            SlicerLayerPath::LayerData layer_data;
            std::vector<geometry::FVector3D> pnt_vec;
            geometry::FVector3D path_center;
            for (int x = 0; x < cur_path.path.size();) {
                geometry::FVector3D f(cur_path.path[x], cur_path.path[x + 1], path.z_height + 0.1);
                path_center += f;
                pnt_vec.push_back(std::move(f));
                x += 2;
            }
            // 计算中心
            path_center /= pnt_vec.size();
            float offset = -0.01;
            if (cur_path.is_ccw_) {
                offset = -offset;
            }
            // 扩散
            for (int x = 0; x < pnt_vec.size(); x++) {
                pnt_vec[x] += ((pnt_vec[x] - path_center) * offset + all_offset_);
            }
            manager->render_engine_interface_->CreateGpuVertexBuffer(
                (const float*)pnt_vec.data(), pnt_vec.size() * 3, layer_data.block_.vertex_handle_);
            layer_data.block_.vertex_size = pnt_vec.size();
            layer_data.is_ccw = cur_path.is_ccw_;
            path.last_layer_index_ = path.layer_index_;
            path.last_z_height = path.z_height;
            path.paths_.push_back(std::move(layer_data));
        }
    };

    auto create_path = [this, manager, create_path_sub](SlicerLayerPath& path, algo::SliceResult::SliceLayerData& dat) {
        if (!path.is_updata_flag_) {
            return;
        }
        for (int i = 0; i < path.paths_.size(); i++) {
            manager->DeleteLineBlock(path.paths_[i].block_);
        }
        path.paths_.clear();

        if(ConfigHelper::GetInstance().GetRenderImp()->GetRenderConfig()->inner_out_slice_path_set_flag_) {
            path.is_updata_flag_ = false;
            create_path_sub(path, dat);
        }else {
            // 0层或者最高层就不显示内外径
            if (path.layer_index_ <= 0 || path.layer_index_ >= path_slice_map_.size()) {
                return;
            }
            auto it = path_slice_map_.find(path.layer_index_);
            if (it != path_slice_map_.end()) {
                path.is_updata_flag_ = false;
                algo::SliceResult* s = it->second;
                algo::SliceResult::SliceLayerData data;
                s->GetSliceLayerInfo(data);
                create_path_sub(path, data);
            }
        }
    };
    if (top_slice_path_.is_updata_flag_) {
        create_path(top_slice_path_, top_path_data_);
    }
    if (bottom_slice_path_.is_updata_flag_) {
        create_path(bottom_slice_path_, bottom_path_data_);
    }
}

std::shared_ptr<SimpleModelBufferEx> SimpleModelBufferEx::createSimpleModelBuffer(
    const std::string& path, bool is_need_place_on_platform_flag /* = true*/) {
    auto modelBuffer = std::make_shared<SimpleModelBufferEx>();
    modelBuffer->pModel = new Model();
    if (!STLMeshLoad::load(path.c_str(), modelBuffer->pModel, is_need_place_on_platform_flag)) {
        delete modelBuffer->pModel;
        modelBuffer->pModel = nullptr;
        return nullptr;
    }
    return modelBuffer;
}

std::shared_ptr<SimpleModelBufferEx> SimpleModelBufferEx::Clone() {
    auto model_buffer = std::make_shared<SimpleModelBufferEx>();
    model_buffer->pModel = new Model();
    *model_buffer->pModel = *this->pModel;
    return model_buffer;
}

void SimpleModelBufferEx::updataRenderBlock(RenderEngineInterface* engine_interface) {
    const auto& vertices = pModel->vertexElement.vertices;
    const auto& indices = pModel->vertexElement.indices;
    auto buffer = std::make_shared<BlockBuffer>(-1, nullptr);

    int vertexBufferSize = vertices.size() * sizeof(FVector3D) / sizeof(float);
    engine_interface->CreateGpuVertexBuffer((float*)vertices.data(), vertexBufferSize, buffer->vertex_handle_);
    engine_interface->CreateGpuIndexBuffer((uint32_t*)indices.data(), indices.size(), buffer->index_handle_);

    buffer->index_size = indices.size();
    buffer->stride_ = sizeof(FVector3D);
    this->buffer = buffer;
}
void BuildPathInfo(ModelRenderManager* manager, algo::CutSidePathInfo::PathInfo* src,
    CutSideIntersectModelPath::CutModelPath::ModelPath* dest) {
    dest->line_blocks_.resize(src->cur_paths_lines_.size());
    for (int i = 0; i < src->cur_paths_lines_.size(); i++) {
        dest->line_blocks_[i] = std::make_shared<VertexBlockBuffer>();
        auto buf = dest->line_blocks_[i];
        auto& line_data_vec = src->cur_paths_lines_[i];
        manager->GetRenderEngineInterface()->CreateGpuVertexBuffer(
            (float*)line_data_vec.data(), line_data_vec.size(), buf->vertex_handle_);
        buf->vertex_size = line_data_vec.size() / 3;
    }
    dest->is_valid_loop_flag_ = src->is_valid_loop_flag_;
    dest->tri_blocks_ = std::make_shared<VertexBlockBuffer>();
    auto& tri_data_vec = src->cur_paths_tris_;
    std::vector<geometry::FVector3D> tri_vec(tri_data_vec.size() / 3);
    memcpy(tri_vec.data(), tri_data_vec.data(), sizeof(float) * tri_data_vec.size());
    manager->GetRenderEngineInterface()->CreateGpuVertexBuffer(
        (float*)tri_data_vec.data(), tri_data_vec.size(), dest->tri_blocks_->vertex_handle_);
    dest->tri_blocks_->vertex_size = tri_data_vec.size() / (3);
}

void CutSideIntersectModelPath::BuildData(ModelRenderManager* manager, algo::AlgoVector<algo::CutSidePathInfo> paths) {
    model_paths_.clear();
    model_paths_.resize(paths.size());
    //模型
    for (int i = 0; i < paths.size(); i++) {
        auto& info = paths[i];
        auto& buf_model_path = model_paths_[i];
        buf_model_path.paths_.resize(info.paths_.size());
        //多个路径
        for (int j = 0; j < info.paths_.size(); j++) {
            algo::CutSidePathInfo::PathInfo& p = info.paths_[j];
            BuildPathInfo(manager, &p, &buf_model_path.paths_[j]);
            buf_model_path.belong_model_id_ = info.belong_model_id_;
        }
    }
}

}  // namespace render

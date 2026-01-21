#include "RenderManager.h"
#include "../../DX11/DX11RenderEngineInterface.h"
#include "../../Core/render_api_impl.h"
#include "Base/MainAreaRender.h"

namespace render {

struct RenderVecStruct {
    bool operator<(const RenderVecStruct& other) const {
        return z_depth_ < other.z_depth_;
    }
    std::shared_ptr<RenderAreaBase> base = nullptr;
    int z_depth_ = 0;
};

RenderManager::RenderManager(RenderApiImpl* impl) {
    render_context_base_ = std::make_shared<RenderContext>(impl, std::make_shared<DX11RenderEngineInterface>());
}

void RenderManager::Render() {
    render_context_base_->UpdateCamera(mouse_control_.GetCamera());
    render_context_base_->UpdateGpuBuffer();
    std::vector<RenderVecStruct> render_vec;
    for(auto& it : this->render_area_map_) {
        RenderVecStruct v;
        v.base = it.second;
        render::RenderItemInfo* info_ptr = v.base->GetRenderItemInfo();
        v.z_depth_ = info_ptr->z_depth_;
        render_vec.push_back(v);
    }
    std::sort(render_vec.begin(), render_vec.end());
    try {
        renderBegin();
        for(int i = 0; i < render_vec.size(); i++) {
            render_vec[i].base->Render();
        }
        renderEnd();

    }catch (const std::exception& e) {
        std::cout << "render get exceptrion e = " << e.what() << std::endl;
    }
}

render::RenderItemInfo* RenderManager::CreateRenderInfo(int id, RenderTargetType type) {
    std::unique_lock<std::mutex> auto_lock(mutex_);
    if (render_area_map_.find(id) == render_area_map_.end()) {
        if(type == RenderTargetType::MainRenderArea)
            render_area_map_[id] = std::make_shared<render::MainAreaRender>(this);
    }
    return render_area_map_[id]->GetRenderItemInfo();
}

void RenderManager::renderBegin() {

}

void RenderManager::renderEnd() {

}
}


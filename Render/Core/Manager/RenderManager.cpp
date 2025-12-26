#include "RenderManager.h"
#include "../../DX11/DX11RenderEngineInterface.h"
#include "../../Core/render_api_impl.h"
#include "Base/RenderAreaBase.h"

namespace render {
RenderManager::RenderManager(RenderApiImpl* impl) {
    render_context_base_ = std::make_shared<RenderContext>(impl, std::make_shared<DX11RenderEngineInterface>());
}

void RenderManager::Render() {
    render_context_base_->UpdateCamera(mouse_control_.GetCamera());
}

render::RenderItemInfo* RenderManager::CreateRenderInfo(int id, RenderTargetType type) {
    std::unique_lock<std::mutex> auto_lock(mutex_);
    if (render_info_map_.find(id) == render_info_map_.end()) {
        render_info_map_[id] = std::make_shared<render::RenderAreaBase>(this);
    }
    return render_info_map_[id]->GetRenderItemInfo();
}
}


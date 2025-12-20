#include "render_api_impl.h"
#include "Manager/RenderManager.h"

namespace render {
RenderApiImpl::RenderApiImpl() {
    render_config_ = std::make_shared<RenderConfig>();
}

void RenderApiImpl::Render(int win_id) {

}

RenderManager* RenderApiImpl::getRenderManager(int win_id, bool not_exist_is_create_flag) {
    std::unique_lock<std::mutex> auto_lock(mutex_);
    if (render_manager_map_.find(win_id) == render_manager_map_.end()) {
        if (!not_exist_is_create_flag) {
            return nullptr;
        }
        std::shared_ptr<RenderManager> render_mgr;
        render_mgr = std::make_shared<RenderManager>(this);
        render_manager_map_[win_id] = render_mgr;
    }
}

}

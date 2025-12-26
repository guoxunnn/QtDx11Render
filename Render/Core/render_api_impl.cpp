#include "render_api_impl.h"
#include "Manager/RenderManager.h"
#include "Manager/RenderMouseAndKeyEventControl.h"
#include "../Core/render_api_impl.h"

namespace render {
RenderApiImpl::RenderApiImpl() {
    render_config_ = std::make_shared<RenderConfig>();
}

std::shared_ptr<RenderApi> RenderApi::CreateRenderImpl() {
    return std::make_shared<RenderApiImpl>();
}

void RenderApiImpl::OnMousePressEvent(const MouseEvent& event) {
    getRenderManager(event.win_id_)->GetMouseControl()->OnMousePressEvent(event);
}

void RenderApiImpl::OnMouseMoveEvent(const MouseEvent& event) {
    getRenderManager(event.win_id_)->GetMouseControl()->OnMouseMoveEvent(event);
}

void RenderApiImpl::OnMouseReleaseEvent(const MouseEvent& event){
    getRenderManager(event.win_id_)->GetMouseControl()->OnMouseReleaseEvent(event);
}

void RenderApiImpl::onMouseClickEvent(const MouseEvent& event){
    getRenderManager(event.win_id_)->GetMouseControl()->onMouseClickEvent(event);
}

void RenderApiImpl::onMouseDoubleClickEvent(const MouseEvent& event){
    getRenderManager(event.win_id_)->GetMouseControl()->onMouseDoubleClickEvent(event);
}

void RenderApiImpl::OnHoverLeaveEvent(const MouseEvent& event){
    getRenderManager(event.win_id_)->GetMouseControl()->OnHoverLeaveEvent(event);
}

void RenderApiImpl::OnHoverMoveEvent(const MouseEvent& event){
    getRenderManager(event.win_id_)->GetMouseControl()->OnHoverMoveEvent(event);
}

void RenderApiImpl::OnUpdataMousePos(const MouseEvent& event){
    getRenderManager(event.win_id_)->GetMouseControl()->OnUpdataMousePos(event);
}

RenderConfig* RenderApiImpl::GetRenderConfig() const {
    return this->render_config_.get();
}

void RenderApiImpl::Render(int win_id) {
    getRenderManager(win_id)->Render();
}

render::RenderInterface* RenderApiImpl::GetRenderEngine(int win_id) {
    return &getRenderManager(win_id)->GetRenderContext()->GetRenderEngine()->render_interface_;
}

render::RenderItemInfo* RenderApiImpl::CreateRenderInfo(int win_id, int base_id, RenderTargetType type) {
    return getRenderManager(win_id)->CreateRenderInfo(base_id, type);
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
    return render_manager_map_[win_id].get();
}
}

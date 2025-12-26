#include "render_manager.h"
#include "render_window_view.h"

RenderManager::RenderManager() {
    render_api_  = render::RenderApi::CreateRenderImpl();
}

void RenderManager::InsertViewMap(QQuickWindow* win, RenderItemBase* renderItem) {
    if (!window_view_map_.contains(win)) {
        RenderWindowView* viewItem = new RenderWindowView(win->winId());
        if (renderItem) {
            viewItem->AddRenderItem(renderItem);
        }
        window_view_map_[win] = viewItem;
    }
}

render::RenderApi* RenderManager::GetRenderApi() {
    return this->render_api_.get();
}

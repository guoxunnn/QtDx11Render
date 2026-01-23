#include "render_window_view.h"
#include "render_area_item.h"
#include <qquickwindow.h>
#include <mutex>
#include "render_manager.h"
#include "../soft_ware_control.h"

RenderWindowView::RenderWindowView(int win_id) {
	connect_flag_ = false;
}

void RenderWindowView::render() {
    auto render_manager = SoftwareControlSingleton::Instance().GetRenderManager();
    std::unique_lock<std::mutex> auto_lock(render_manager->render_mutex_);
    belong_window_->beginExternalCommands();
    {
        QSGRendererInterface* rendererInterface = belong_window_->rendererInterface();
        render::RenderInterface* render_interface = render_manager->GetRenderApi()->GetRenderEngine(belong_window_->winId());
#if defined(_WIN32) || defined(_WIN32_) || defined(WIN32) || defined(_WIN64_) || defined(WIN64) || defined(_WIN64)
        {
            render_interface->drv = reinterpret_cast<ID3D11Device*>(
                rendererInterface->getResource(belong_window_, QSGRendererInterface::DeviceResource));
            render_interface->drv_context = reinterpret_cast<ID3D11DeviceContext*>(
                rendererInterface->getResource(belong_window_, QSGRendererInterface::DeviceContextResource));
        }
#endif
    }
    render_manager->GetRenderApi()->Render(belong_window_->winId());
    belong_window_->endExternalCommands();
}

void RenderWindowView::sizeChanged() {

}

void RenderWindowView::AddRenderItem(RenderItemBase* item) {
	if (!connect_flag_) {
		connect_flag_ = true;
        connect(item, &RenderItemBase::renderRequest, this, &RenderWindowView::render, Qt::DirectConnection);
        belong_window_ = item->window();
        auto win = belong_window_;
        connect(win, &QQuickWindow::widthChanged, this, &RenderWindowView::sizeChanged);
        connect(win, &QQuickWindow::heightChanged, this, &RenderWindowView::sizeChanged);
	}
}

void RenderWindowView::init() {
    if (init_flag_)
        return;
    init_flag_ = true;
}

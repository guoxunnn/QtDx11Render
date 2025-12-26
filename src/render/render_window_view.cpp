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
    render_manager->GetRenderApi()->Render(belong_window_->winId());
    belong_window_->endExternalCommands();
}

void RenderWindowView::sizeChanged() {

}

void RenderWindowView::AddRenderItem(RenderItemBase* item) {
	if (!connect_flag_) {
		connect_flag_ = true;
        connect(item, &RenderItemBase::renderRequest, this, &RenderWindowView::render, Qt::DirectConnection);
        auto win = belong_window_;
        belong_window_ = item->window();
        connect(win, &QQuickWindow::widthChanged, this, &RenderWindowView::sizeChanged);
        connect(win, &QQuickWindow::heightChanged, this, &RenderWindowView::sizeChanged);
	}
}

void RenderWindowView::init() {
    if (init_flag_)
        return;
    init_flag_ = true;
}

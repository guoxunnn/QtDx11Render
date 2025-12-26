#include "render_area_item.h"
#include <QQuickWindow>
#include "../soft_ware_control.h"
#include <QPoint>

static std::atomic_int64_t id_count = 0;

RenderItemBase::RenderItemBase() : id_(id_count.fetch_add(1)) {
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);
    setFlag(ItemAcceptsDrops, true);
    setFocus(true);
    connect(this, &QQuickItem::windowChanged, this, [this](QQuickWindow* window) {
        if (window) {
            cur_belong_win_ = window;
            auto render_manager = SoftwareControlSingleton::Instance().GetRenderManager();
            render_info_ = render_manager->GetRenderApi()->CreateRenderInfo(window->winId(), id_);
            render_manager->InsertViewMap(window, this);
            connect(window, &QQuickWindow::sceneGraphInvalidated, this, [](){

            }, Qt::DirectConnection);
            connect(window, &QQuickWindow::destroyed, this, [=]() {
            });
            connect(window, &QQuickWindow::beforeSynchronizing, this, [this](){
                if (init_flag_) {
                    auto render_cb = [this]() {
                        onXYChange();
                        // 发出渲染请求
                        emit renderRequest();
                    };
                    if (before_ui_render_flag_) {
                        connect(this->window(), &QQuickWindow::beforeRenderPassRecording, this, [render_cb](){
                            render_cb();
                        },Qt::DirectConnection);
                    }
                    else {
                        connect(this->window(), &QQuickWindow::afterRenderPassRecording, this, [render_cb](){
                            render_cb();
                        },Qt::DirectConnection);
                    }
                    init_flag_ = false;
                }
            }, Qt::DirectConnection);
        }
    });
}

void RenderItemBase::onXYChange() {
    int x = 0;
    int y = 0;
    QQuickItem* item = this;
    while (item != nullptr) {
        x += item->x();
        y += item->y();
        item = item->parentItem();
    }
    render_info_->x_ = x * window()->devicePixelRatio();
    render_info_->y_ = y * window()->devicePixelRatio();
    render_info_->width_ = width() * window()->devicePixelRatio();
    render_info_->height_ = height() * window()->devicePixelRatio();
}

render::MouseEvent RenderItemBase::getRenderMouseEvent(QMouseEvent* event, bool is_release) {
    render::MouseEvent res;
    if (is_release) {
        if (event->button() & Qt::LeftButton)
            res.buttons |= render::MouseEvent::LeftButton;
        if (event->button() & Qt::RightButton)
            res.buttons |= render::MouseEvent::RightButton;
        if (event->button() & Qt::MiddleButton)
            res.buttons |= render::MouseEvent::MiddleButton;
    }
    else {
        if (event->buttons() & Qt::LeftButton)
            res.buttons |= render::MouseEvent::LeftButton;
        if (event->buttons() & Qt::RightButton)
            res.buttons |= render::MouseEvent::RightButton;
        if (event->buttons() & Qt::MiddleButton)
            res.buttons |= render::MouseEvent::MiddleButton;
        if (res.buttons == 0 && event->button() != 0) {
            res.buttons = event->button();
        }
    }
    res.device_pixel_ratio_ = window()->devicePixelRatio();
    res.position.x = event->pos().x() * res.device_pixel_ratio_;
    res.position.y = event->pos().y() * res.device_pixel_ratio_;
    if (render_info_) {
        res.start_pos_.x = render_info_->x_;
        res.start_pos_.y = render_info_->y_;
    }
    res.viewport_h_ = height();
    res.viewport_w_ = width();
    res.win_id_ = window()->winId();
    return res;
}

RenderAreaItem::RenderAreaItem() {
}

void RenderAreaItem::mousePressEvent(QMouseEvent* event) {
    auto render_api = SoftwareControlSingleton::Instance().GetRenderManager()->GetRenderApi();
    auto render_mouse_event = getRenderMouseEvent(event);
    render_api->OnMousePressEvent(render_mouse_event);
}

void RenderAreaItem::mouseMoveEvent(QMouseEvent* event) {
    auto render_api = SoftwareControlSingleton::Instance().GetRenderManager()->GetRenderApi();
    auto render_mouse_event = getRenderMouseEvent(event);
    render_api->OnMouseMoveEvent(render_mouse_event);
}

void RenderAreaItem::mouseReleaseEvent(QMouseEvent* event) {
    auto render_api = SoftwareControlSingleton::Instance().GetRenderManager()->GetRenderApi();
    auto render_mouse_event = getRenderMouseEvent(event);
    render_api->OnMouseReleaseEvent(render_mouse_event);
}

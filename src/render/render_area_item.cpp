#include "render_area_item.h"
#include <QQuickWindow>
#include "../soft_ware_control.h"

RenderItemBase::RenderItemBase() {
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);
    setFlag(ItemAcceptsDrops, true);
    setFocus(true);
    connect(this, &QQuickItem::windowChanged, this, [this](QQuickWindow* window) {
        if (window) {
            cur_belong_win_ = window;
            SoftwareControlSingleton::Instance().GetRenderManager()->InsertViewMap(window, this);
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
//    if (x != renderX || y != renderY) {
//        setrenderX(x);
//        setrenderY(y);
//    }
}

RenderAreaItem::RenderAreaItem() {

}

void RenderAreaItem::mousePressEvent(QMouseEvent* event) {

}

void RenderAreaItem::mouseMoveEvent(QMouseEvent* event) {

}

void RenderAreaItem::mouseReleaseEvent(QMouseEvent* event) {

}

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
            connect(window, &QQuickWindow::beforeSynchronizing, this, [](){

            }, Qt::DirectConnection);
        }
    });
}

RenderAreaItem::RenderAreaItem() {

}

void RenderAreaItem::mousePressEvent(QMouseEvent* event) {

}

void RenderAreaItem::mouseMoveEvent(QMouseEvent* event) {

}

void RenderAreaItem::mouseReleaseEvent(QMouseEvent* event) {

}

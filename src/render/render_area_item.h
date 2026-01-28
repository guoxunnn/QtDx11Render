#ifndef RENDERAREAITEM_H
#define RENDERAREAITEM_H
#include <QQuickItem>
#include "../client_macro.h"
#include "Render/Impl/render_data.h"

class RenderItemBase : public QQuickItem {
    Q_OBJECT

    REGISTER_Q_PROPERTY(int, z_depth, 0)
public:
    RenderItemBase();
    int64_t ID() {
        return id_;
    }

protected:
    void onXYChange();
    render::MouseEvent getRenderMouseEvent(QMouseEvent* event, bool is_release = false);

signals:
    void renderRequest();

protected:
    bool init_flag_ = true;
    QQuickWindow* cur_belong_win_ = nullptr;
    bool before_ui_render_flag_ = true;
    int64_t id_ = -1;
    render::RenderItemInfo* render_info_ = nullptr;
};

class RenderAreaItem : public RenderItemBase {
    Q_OBJECT

public:
    RenderAreaItem();
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
};

#endif // RENDERAREAITEM_H

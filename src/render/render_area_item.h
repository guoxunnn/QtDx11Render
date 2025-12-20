#ifndef RENDERAREAITEM_H
#define RENDERAREAITEM_H
#include <QQuickItem>
#include "../client_macro.h"

class RenderItemBase : public QQuickItem {
    Q_OBJECT

public:
    RenderItemBase();

protected:
    void onXYChange();

signals:
    void renderRequest();

protected:
    bool init_flag_ = true;
    QQuickWindow* cur_belong_win_ = nullptr;
    bool before_ui_render_flag_ = true;
};

class RenderAreaItem : public RenderItemBase {
    Q_OBJECT

public:
    RenderAreaItem();
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif // RENDERAREAITEM_H

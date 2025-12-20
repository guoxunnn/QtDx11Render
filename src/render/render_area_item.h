#ifndef RENDERAREAITEM_H
#define RENDERAREAITEM_H
#include <QQuickItem>
class RenderItemBase : public QQuickItem {
    Q_OBJECT
public:
    RenderItemBase();

protected:
    QQuickWindow* cur_belong_win_ = nullptr;
};

class RenderAreaItem : public RenderItemBase {
    Q_OBJECT

public:
    RenderAreaItem();
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
};

#endif // RENDERAREAITEM_H

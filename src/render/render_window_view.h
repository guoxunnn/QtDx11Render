#ifndef RENDERWINDOWVIEW_H
#define RENDERWINDOWVIEW_H
#include <QObject>
#include <QQuickWindow>

class RenderItemBase;

class RenderWindowView : public QObject {
    Q_OBJECT

public:
    RenderWindowView(int win_id);
    void AddRenderItem(RenderItemBase* item);

protected:
    void init();

protected slots:
    void render();
    void sizeChanged();

private:
    QQuickWindow* belong_window_ = nullptr;
    std::vector<RenderItemBase*> renderItems;
    std::atomic_bool connect_flag_;
    bool init_flag_ = false;
};

#endif // RENDERWINDOWVIEW_H

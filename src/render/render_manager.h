#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H
#include "../singleton.h"
#include "Render/Impl/render_api.h"
#include <QObject>
#include <QQuickWindow>
class RenderItemBase;
class RenderWindowView;

class RenderManager : public QObject {
    Q_OBJECT

public:
    RenderManager();
    void InsertViewMap(QQuickWindow* win, RenderItemBase* renderItem);

private:
    std::shared_ptr<render::RenderApi> render_api_ = nullptr;
    QMap<QQuickWindow*, RenderWindowView*> window_view_map_;
};

#endif // RENDERMANAGER_H

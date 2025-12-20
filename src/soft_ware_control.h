#ifndef SOFTWARECONTROL_H
#define SOFTWARECONTROL_H
#include "singleton.h"
#include "render/render_manager.h"
#include <QObject>

class SoftwareControl : public QObject {
    Q_OBJECT

public:
    SoftwareControl();
    void Init();

public slots:
    RenderManager* GetRenderManager();

private:
    std::shared_ptr<RenderManager> render_manager_ = nullptr;
};
using SoftwareControlSingleton = base::Singleton<SoftwareControl>;

#endif // SOFTWARECONTROL_H

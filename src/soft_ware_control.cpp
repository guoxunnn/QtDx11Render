#include "soft_ware_control.h"
#include <qqml.h>
#include "render/render_area_item.h"

SoftwareControl::SoftwareControl() {
    render_manager_ = std::make_shared<RenderManager>();
}

void SoftwareControl::Init() {
    qmlRegisterType<RenderAreaItem>("Test", 1, 0, "RenderAreaItem");
    qmlRegisterType<RenderManager>("Test", 1, 0, "RenderManager");
    qmlRegisterType<SoftwareControl>("Test", 1, 0, "SoftwareControl");
    qmlRegisterSingletonInstance<SoftwareControl>("Test", 1, 0, "SoftwareControlSingleton", &SoftwareControlSingleton::Instance());
}

RenderManager* SoftwareControl::GetRenderManager() {
    return this->render_manager_.get();
}

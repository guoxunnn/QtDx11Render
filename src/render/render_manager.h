#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H
#include "../singleton.h"
#include "Render/Impl/render_api.h"

class RenderManager {
public:
    RenderManager();

private:
    std::shared_ptr<render::RenderApi> render_api_ = nullptr;
};
using RenderManagerSingleton = base::Singleton<RenderManager>;

#endif // RENDERMANAGER_H

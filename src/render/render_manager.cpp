#include "render_manager.h"

RenderManager::RenderManager() {
    render_api_  = render::RenderApi::CreateRenderImpl();
}

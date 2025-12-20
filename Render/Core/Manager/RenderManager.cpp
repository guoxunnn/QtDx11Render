#include "RenderManager.h"
#include "../../DX11/DX11RenderEngineInterface.h"
#include "../../Core/render_api_impl.h"

namespace render {
RenderManager::RenderManager(RenderApiImpl* impl) {
    render_context_base_ = std::make_shared<RenderContext>(impl, std::make_shared<DX11RenderEngineInterface>());
}

void RenderManager::Render() {
    render_context_base_->UpdateCamera(mouse_control_.GetCamera());
}
}


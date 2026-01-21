#include "RenderContext.h"
namespace render {
RenderContext::RenderContext(RenderApiImpl* impl, std::shared_ptr<RenderEngineInterface> engine):impl_(impl), engine_(engine) {

}

void RenderContext::UpdateCamera(const RCamera& c) {
    this->render_camera_ = c;
}

void RenderContext::UpdateGpuBuffer() {
    GetModelRenderManager()->BuildModelGpuBuffer(shared_from_this());
}
}


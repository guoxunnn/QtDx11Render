#ifndef RENDERCONTEXT_H
#define RENDERCONTEXT_H
#include "../Core/render_api_impl.h"
#include "../Base/RenderEngineInterface.h"
#include "RCamera.h"
#include "../Core/Manager/ModelRenderManager.h"

namespace  render {
class RenderContext : public std::enable_shared_from_this<RenderContext> {
public:
    RenderContext(RenderApiImpl* impl, std::shared_ptr<RenderEngineInterface> engine);
    void UpdateCamera(const RCamera& c);
    void UpdateGpuBuffer();
    RenderEngineInterface* GetRenderEngine() { return engine_.get(); }
    ModelRenderManager* GetModelRenderManager(){return impl_->GetModelRenderManager();}

private:
    RenderApiImpl* impl_;
    std::shared_ptr<RenderEngineInterface> engine_;
    //渲染摄像机
    RCamera render_camera_;
};
}


#endif // RENDERCONTEXT_H

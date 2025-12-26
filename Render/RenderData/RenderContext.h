#ifndef RENDERCONTEXT_H
#define RENDERCONTEXT_H
#include "../Core/render_api_impl.h"
#include "../Base/RenderEngineInterface.h"
#include "RCamera.h"

namespace  render {
class RenderContext {
public:
    RenderContext(RenderApiImpl* impl, std::shared_ptr<RenderEngineInterface> engine);
    void UpdateCamera(const RCamera& c);
    RenderEngineInterface* GetRenderEngine() { return engine_.get(); }

private:
    RenderApiImpl* impl_;
    std::shared_ptr<RenderEngineInterface> engine_;
    //渲染摄像机
    RCamera render_camera_;
};
}


#endif // RENDERCONTEXT_H

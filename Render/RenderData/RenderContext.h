#ifndef RENDERCONTEXT_H
#define RENDERCONTEXT_H
#include "../Core/render_api_impl.h"
#include "../Base/RenderEngineInterface.h"

namespace  render {
class RenderContext
{
public:
    RenderContext(RenderApiImpl* impl, std::shared_ptr<RenderEngineInterface> engine);

private:
    RenderApiImpl* impl_;
    std::shared_ptr<RenderEngineInterface> engine_;
};
}


#endif // RENDERCONTEXT_H

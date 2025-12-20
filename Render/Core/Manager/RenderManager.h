#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H
#include <memory>
#include "../../RenderData/RenderContext.h"

namespace render {
class RenderApiImpl;

class RenderManager {
public:
    RenderManager(RenderApiImpl* impl);

protected:
    std::shared_ptr<RenderContext> render_context_base_ = nullptr;
};
}


#endif // RENDERMANAGER_H

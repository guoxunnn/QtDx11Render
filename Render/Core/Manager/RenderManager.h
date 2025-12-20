#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H
#include <memory>
#include "../../RenderData/RenderContext.h"
#include "RenderMouseAndKeyEventControl.h"

namespace render {
class RenderApiImpl;

class RenderManager {
public:
    RenderManager(RenderApiImpl* impl);
    void Render();
    RenderMouseAndKeyEventControl* GetMouseControl() {
        return &mouse_control_;
    }
protected:
    std::shared_ptr<RenderContext> render_context_base_ = nullptr;
    RenderMouseAndKeyEventControl mouse_control_;
};
}


#endif // RENDERMANAGER_H

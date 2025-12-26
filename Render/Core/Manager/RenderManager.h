#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H
#include <memory>
#include "../../RenderData/RenderContext.h"
#include "RenderMouseAndKeyEventControl.h"

namespace render {
class RenderApiImpl;
class RenderBase;
class RenderAreaBase;

class RenderManager {
public:
    RenderManager(RenderApiImpl* impl);
    void Render();
    RenderMouseAndKeyEventControl* GetMouseControl() {
        return &mouse_control_;
    }
    std::shared_ptr<RenderContext> GetRenderContext() { return render_context_base_; }
    render::RenderItemInfo* CreateRenderInfo(int id, render::RenderTargetType type);

protected:
    void renderBegin();
    void renderEnd();

protected:
    std::shared_ptr<RenderContext> render_context_base_ = nullptr;
    RenderMouseAndKeyEventControl mouse_control_;
    std::unordered_map<int, std::shared_ptr<render::RenderAreaBase>> render_area_map_;

private:
    std::mutex mutex_;
};
}


#endif // RENDERMANAGER_H

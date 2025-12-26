#ifndef RENDER_API_IMPL_H
#define RENDER_API_IMPL_H
#include "../Impl/render_api.h"
#include <memory>
#include <mutex>
#include <map>

namespace render {
class RenderManager;

class RenderApiImpl : public RenderApi {
public:
    RenderApiImpl();
    void OnMousePressEvent(const MouseEvent& event) override;
    void OnMouseMoveEvent(const MouseEvent& event) override;
    void OnMouseReleaseEvent(const MouseEvent& event) override;
    void onMouseClickEvent(const MouseEvent& event) override;
    void onMouseDoubleClickEvent(const MouseEvent& event) override;
    void OnHoverLeaveEvent(const MouseEvent& event) override;
    void OnHoverMoveEvent(const MouseEvent& event) override;
    void OnUpdataMousePos(const MouseEvent& event) override;
    RenderConfig* GetRenderConfig() const override;
    void Render(int win_id) override;
    virtual render::RenderInterface* GetRenderEngine(int win_id);

private:
    RenderManager* getRenderManager(int win_id, bool not_exist_is_create_flag = true);

public:
    std::shared_ptr<RenderConfig> render_config_ = nullptr;
    std::map<int, std::shared_ptr<RenderManager>> render_manager_map_;
    std::mutex mutex_;
};
}


#endif // RENDER_API_IMPL_H

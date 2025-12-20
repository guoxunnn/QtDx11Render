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
    void OnMousePressEvent(const MouseEvent& event);
    void OnMouseMoveEvent(const MouseEvent& event);
    void OnMouseReleaseEvent(const MouseEvent& event);
    void onMouseClickEvent(const MouseEvent& event);
    void onMouseDoubleClickEvent(const MouseEvent& event);
    void OnHoverLeaveEvent(const MouseEvent& event);
    void OnHoverMoveEvent(const MouseEvent& event);
    void OnUpdataMousePos(const MouseEvent& event);
    RenderConfig* GetRenderConfig() const;
    void Render(int win_id) override;

private:
    RenderManager* getRenderManager(int win_id, bool not_exist_is_create_flag = true);

public:
    std::shared_ptr<RenderConfig> render_config_ = nullptr;
    std::map<int, std::shared_ptr<RenderManager>> render_manager_map_;
    std::mutex mutex_;
};
}


#endif // RENDER_API_IMPL_H

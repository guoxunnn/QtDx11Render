#ifndef RENDER_API_H
#define RENDER_API_H
#include "render_config.h"
#include "render_data.h"
#include <memory>

namespace render {
class RenderApi {
public:
    RenderApi();
    static std::shared_ptr<RenderApi> CreateRenderImpl();
    virtual void OnMousePressEvent(const MouseEvent& event) = 0;
    virtual void OnMouseMoveEvent(const MouseEvent& event) = 0;
    virtual void OnMouseReleaseEvent(const MouseEvent& event) = 0;
    virtual void onMouseClickEvent(const MouseEvent& event) = 0;
    virtual void onMouseDoubleClickEvent(const MouseEvent& event) = 0;
    virtual void OnHoverLeaveEvent(const MouseEvent& event) = 0;
    virtual void OnHoverMoveEvent(const MouseEvent& event) = 0;
    virtual void OnUpdataMousePos(const MouseEvent& event) = 0;
    virtual void Render(int win_id) = 0;
    virtual render::RenderInterface* GetRenderEngine(int win_id) = 0;
    virtual RenderConfig* GetRenderConfig() const = 0;
    //返回的指针归属权属于渲染模块管理
    virtual render::RenderItemInfo* CreateRenderInfo(int win_id, int base_id) = 0;
};
}


#endif // RENDER_API_H

#ifndef RENDERMOUSEANDKEYEVENTCONTROL
#define RENDERMOUSEANDKEYEVENTCONTROL
#include "Render/Impl/render_data.h"
#include "Render/RenderData/RCamera.h"

namespace render {
class RenderMouseAndKeyEventControl {
public:
    RenderMouseAndKeyEventControl();
    void OnMousePressEvent(const MouseEvent& event);
    void OnWheelEvent(const WheelEvent& event);
    void OnMouseMoveEvent(const MouseEvent& event);
    void OnMouseReleaseEvent(const MouseEvent& event);
    void onMouseClickEvent(const MouseEvent& event);
    void onMouseDoubleClickEvent(const MouseEvent& event);
    void OnHoverLeaveEvent(const MouseEvent& event);
    void OnHoverMoveEvent(const MouseEvent& event);
    void OnUpdataMousePos(const MouseEvent& event);
    const RCamera& GetCamera() const {
        return cur_camera_;
    }
private:
    RCamera cur_camera_;
    // 鼠标的上一个位置
    Point last_pos_;
};
}


#endif // RENDERMOUSEANDKEYEVENTCONTROL

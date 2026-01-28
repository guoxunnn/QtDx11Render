#include "RenderMouseAndKeyEventControl.h"
namespace render {
float slowly_factor = 0.15;

RenderMouseAndKeyEventControl::RenderMouseAndKeyEventControl()
{

}

void RenderMouseAndKeyEventControl::OnMousePressEvent(const MouseEvent& event) {
    last_pos_ = event.position;
}

void RenderMouseAndKeyEventControl::OnWheelEvent(const WheelEvent& event) {
    bool is_slowly = event.modifiers_ & MouseEvent::ControlModifier;
    int mouse_x = event.position.x;
    int mouse_y = event.position.y;
    int factor = 1 + event.angleDelta * event.det * 0.5;
    if (is_slowly) {
        factor = (factor - 1.0) * slowly_factor + 1.0;
    }
    cur_camera_.ZoomAtPoint(geometry::DVector2D(mouse_x, mouse_y), factor);
}

void RenderMouseAndKeyEventControl::OnMouseMoveEvent(const MouseEvent& event) {
    bool is_slowly = event.modifiers_ & MouseEvent::ControlModifier;
    int dx = event.position.x - last_pos_.x;
    int dy = event.position.y - last_pos_.y;
    if(event.buttons == MouseEvent::LeftButton) {
        double factor = is_slowly ? slowly_factor : 1.0;
        cur_camera_.Pitch(0.5 * dy * factor, true);
        cur_camera_.RollOnWorld(0.5 * dx * factor, true);
    }else if(event.buttons == MouseEvent::MiddleButton){
        double factor = is_slowly ? slowly_factor : 1.0;
        geometry::DVector3D ref_pos = cur_camera_.GetForwardDirection() * Perspective_Zoom_Max_Negative_Depth;
        geometry::DVector3D last_pos = cur_camera_.ViewportPointToClip(geometry::DVector2D(last_pos_.x, last_pos_.y), ref_pos);
        geometry::DVector3D curr_pos = cur_camera_.ViewportPointToClip(geometry::DVector2D(event.position.x, event.position.y), ref_pos);
        cur_camera_.Pan(geometry::DVector2D(curr_pos - last_pos) * factor);
    }
    else if (event.buttons == MouseEvent::RightButton) {
        double factor = is_slowly ? slowly_factor : 1.0;
        cur_camera_.Pitch(0.5 * dy * factor, true);
        cur_camera_.RollOnWorld(0.5 * dx * factor, true);
    }
    last_pos_ = event.position;
}

void RenderMouseAndKeyEventControl::OnMouseReleaseEvent(const MouseEvent& event) {

}

void RenderMouseAndKeyEventControl::onMouseClickEvent(const MouseEvent& event) {

}

void RenderMouseAndKeyEventControl::onMouseDoubleClickEvent(const MouseEvent& event) {

}

void RenderMouseAndKeyEventControl::OnHoverLeaveEvent(const MouseEvent& event) {

}

void RenderMouseAndKeyEventControl::OnHoverMoveEvent(const MouseEvent& event) {

}

void RenderMouseAndKeyEventControl::OnUpdataMousePos(const MouseEvent& event) {

}
}


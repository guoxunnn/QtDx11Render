
#include "MainAreaRender.h"
#include "../Core/Render/ModelRender.h"

namespace render {
void MainAreaRender::Render() {
    RenderEngineInterface::Viewport view_port;
    view_port.x = render_item_info_->x_;
    view_port.y = render_item_info_->y_;
    view_port.w = render_item_info_->width_;
    view_port.h = render_item_info_->height_;
    GetRenderContext()->GetRenderEngineInterface()->SetViewport(view_port);
    GetRenderContext()->GetRenderEngineInterface()->ClearCurColor(RenderColor::FromColor32(90, 90, 90));
    FindRenderBase<ModelRender>(RenderType::NormalModel)->Render();
}
}



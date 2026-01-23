
#include "MainAreaRender.h"
#include "../Core/Render/ModelRender.h"

namespace render {
void MainAreaRender::Render() {
    GetRenderContext()->GetRenderEngineInterface()->ClearCurColor(RenderColor::FromColor32(90, 90, 90));
    //FindRenderBase<ModelRender>(RenderType::NormalModel)->Render();
}
}



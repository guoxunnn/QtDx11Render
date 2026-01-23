
#include "MainAreaRender.h"

namespace render {
void MainAreaRender::Render() {
    GetRenderContext()->GetRenderEngineInterface()->ClearCurColor(RenderColor::FromColor32(90, 90, 90));
}
}



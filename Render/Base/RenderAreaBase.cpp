
#include "RenderAreaBase.h"
#include "Core/Manager/RenderManager.h"


namespace render {
RenderAreaBase::RenderAreaBase(RenderManager* manager) {
    render_manager_ = manager;
    render_item_info_ = std::make_shared<RenderItemInfo>();
}

std::shared_ptr<RenderContext> RenderAreaBase::GetRenderContext() {
    return render_manager_->GetRenderContext();
}
}



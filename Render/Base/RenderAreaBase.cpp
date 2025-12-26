
#include "RenderAreaBase.h"
namespace render {
RenderAreaBase::RenderAreaBase(RenderManager* manager) {
    render_manager_ = manager;
    render_item_info_ = std::make_shared<RenderItemInfo>();
}
}



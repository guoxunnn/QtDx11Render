
#ifndef RENDERAREABASE_H
#define RENDERAREABASE_H
#include "Impl/render_data.h"
#include <memory>

namespace render {
class RenderManager;

class RenderAreaBase {
public:
    RenderAreaBase(RenderManager* manager);
    RenderItemInfo* GetRenderItemInfo() {return render_item_info_.get();}
    virtual void Render() = 0;

protected:
    std::shared_ptr<RenderItemInfo> render_item_info_ = nullptr;
    RenderManager* render_manager_ = nullptr;
};
}


#endif // RENDERAREABASE_H

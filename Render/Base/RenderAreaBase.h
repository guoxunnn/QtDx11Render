
#ifndef RENDERAREABASE_H
#define RENDERAREABASE_H
#include "Impl/render_data.h"
#include <memory>
#include "RenderData/RenderContext.h"

namespace render {
class RenderManager;
class RenderContext;
class RenderBase;

class RenderAreaBase {
public:
    RenderAreaBase(RenderManager* manager);
    RenderItemInfo* GetRenderItemInfo() {return render_item_info_.get();}
    virtual void Render() = 0;
    std::shared_ptr<RenderContext> GetRenderContext();
    const RenderItemInfo& AreaInfo() const {return *render_item_info_.get();}
    enum RenderType {
        NormalModel
    };

    template <class T>
    T* FindRenderBase(RenderType render_type) {
        auto it = render_base_map_.find(render_type);
        if (it != render_base_map_.end()) {
            return reinterpret_cast<T*>(render_base_map_[render_type].get());
        }
        else {
            auto renderPtr = std::make_shared<T>(this);
            render_base_map_[render_type] = renderPtr;
            return renderPtr.get();
        }
    }
protected:
    std::map<RenderType, std::shared_ptr<RenderBase>> render_base_map_;
    std::shared_ptr<RenderItemInfo> render_item_info_ = nullptr;
    RenderManager* render_manager_ = nullptr;
};
}


#endif // RENDERAREABASE_H

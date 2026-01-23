
#ifndef MODELRENDER_H
#define MODELRENDER_H
#include "../../Base/RenderBase.h"



namespace render {
namespace ModelRenderCB {
struct ColorCB {
    RenderMaterialConfig mat;
    RenderLightConfig light;
};

struct ParameterCB {
};
}

class ModelRender : public RenderBase {
public:
    ModelRender(RenderAreaBase* engine);
    bool init() override;

private:
    RenderConstObject<ModelRenderCB::ColorCB> color_cb_;
    RenderConstObject<ModelRenderCB::ParameterCB> param_cb_;
    RenderProgramBase::ShaderInfo shader_info_;
};
}

#endif // MODELRENDER_H

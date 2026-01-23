
#ifndef MODELRENDER_H
#define MODELRENDER_H
#include "../../Base/RenderBase.h"



namespace render {
namespace ModelRenderCB {
struct ColorCB {
    ColorCB() {
        mat = RenderColor::FromColor32(12, 136, 224);
        light.ambient_ = RenderColor::FromColor32(50, 50, 50);
        light.diffuse_ = RenderColor::FromColor32(150, 150, 150);
        light.specular_ = RenderColor::FromColor32(90, 90, 90);
        light.pos_[0] = 320;
        light.pos_[1] = 320;
        light.pos_[3] = 1000;
    }
    RenderMaterialConfig mat;
    RenderLightConfig light;
};

struct ParameterCB {
};
}

class ModelRender : public RenderBase {
public:
    ModelRender(RenderAreaBase* engine);
    void Render();

protected:
    bool init() override;
    void bind(std::shared_ptr<ModelBuffer> model_buf);
    void updataConstVar();

private:
    RenderConstObject<ModelRenderCB::ColorCB> color_cb_;
    RenderConstObject<ModelRenderCB::ParameterCB> param_cb_;
    RenderProgramBase::ShaderInfo shader_info_;
};
}

#endif // MODELRENDER_H

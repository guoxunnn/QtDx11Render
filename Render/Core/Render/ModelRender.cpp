
#include "ModelRender.h"
namespace render {
ModelRender::ModelRender(RenderAreaBase *engine) :RenderBase(engine) {

}

bool ModelRender::init() {
    RenderBase::init();
    color_cb_.setBuffer(getRenderEngineInterface()->CreateConstBuffer(1, color_cb_.data_size_, "ColorCB"));
    param_cb_.setBuffer(getRenderEngineInterface()->CreateConstBuffer(2, param_cb_.data_size_, "ParamCB"));
    shader_info_.pragma_name_ = "Model";
    shader_program_ = getRenderEngineInterface()->CreateShaderProgramCB(shader_info_);
    return true;
}
}


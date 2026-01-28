
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

void ModelRender::Render() {
    renderInit();
    auto model_buf_map = render_context_->GetModelRenderManager()->GetTestModelBuffer();
    for(auto it: model_buf_map) {
        bind(it.second);
        renderModelBuffer(it.second.get());
    }
    release();
}

void ModelRender::bind(std::shared_ptr<ModelBuffer> model_buf) {
    const auto& r_c = render_context_->GetCamera();
    mat_cb_.data.modelMatrix = model_buf->model_matrix_;
    mat_cb_.data.projMatrix = r_c.GetProjectMatrix();
    mat_cb_.data.worldMatrix = r_c.GetViewMatrix();
    mat_cb_.data.modelNormalMatrix = model_buf->model_matrix_.normalMatrix();
    mat_cb_.data.worldNormalMatrix = r_c.GetViewMatrix().normalMatrix();
    auto final_mat = r_c.GetProjectMatrix() * r_c.GetViewMatrix() * model_buf->model_matrix_;
    mat_cb_.data.finallyMatrix = final_mat;
    updataConstVar();
    setStatusInfo();
}

void ModelRender::setStatusInfo() {
    getRenderEngineInterface()->UseShaderProgramCB(shader_program_);
    getRenderEngineInterface()->SetRenderStatusCB(status_info_);

}

void ModelRender::updataConstVar() {
    RenderBase::updataConstVar();
}

}


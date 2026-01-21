
#include "RenderBase.h"
#include "RenderAreaBase.h"

namespace render {
RenderBase::RenderBase(RenderAreaBase* engine) {
    render_engine_ = engine;
}

void RenderBase::updataConstVar(){
    getRenderEngineInterface()->UpdataConstBufferCB(mat_cb_.getBuffer());
}
void RenderBase::release(){
    status_info_.cull_back = false;
    status_info_.transparent_blend = false;
    status_info_.no_color_write_blend = false;
    status_info_.depth_stencil_type = RenderStatusInfo::Deafualt_Type;
    getRenderEngineInterface()->BindTexture(0, nullptr, nullptr);
    getRenderEngineInterface()->BindTexture(1, nullptr, nullptr);
    getRenderEngineInterface()->BindTexture(2, nullptr, nullptr);
    getRenderEngineInterface()->BindTexture(3, nullptr, nullptr);
    getRenderEngineInterface()->BindTexture(4, nullptr, nullptr);
    getRenderEngineInterface()->BindTexture(5, nullptr, nullptr);
    getRenderEngineInterface()->BindTexture(6, nullptr, nullptr);
    getRenderEngineInterface()->BindTexture(7, nullptr, nullptr);
}
void RenderBase::renderInit() {
    if (is_init_flag_) {
        return;
    }
    is_init_flag_ = true;
    init();
}
std::shared_ptr<RenderEngineInterface> RenderBase::getRenderEngineInterface() const{
    return render_engine_->GetRenderContext()->GetRenderEngineInterface();
}

std::shared_ptr<RenderUavBase> RenderBase::CreateShaderPickUav(int slot){
    return getRenderEngineInterface()->CreateUav(1, sizeof(ShaderPickInfo), slot, true);
}

void RenderBase::BindShaderPickUav(std::shared_ptr<RenderUavBase> ptr){
//    Point currentMousePos =
//        render_context_->mouse_and_key_event_control_.GetCurRenderMouseEvent().GetGlobalRenderMousePos();
//    Rect sz = render_engine_->GetRenderInfo().windowInfo.windowSize;
//    mat_cb_.data.uav_pos_x = currentMousePos.x + sz.x;
//    mat_cb_.data.uav_pos_y = currentMousePos.y + sz.y;
//    mat_cb_.data.uav_pick_flag_ = 1;
//    mat_cb_.data.win_height = render_context_->client_info_->windowInfo.GetWindowHeight();
//    getRenderEngineInterface()->BindUavViews({ptr});
}
ShaderPickInfo RenderBase::GetPickInfo(std::shared_ptr<RenderUavBase> v){
    ShaderPickInfo res;
    void* ptr = getRenderEngineInterface()->MapUav(v);
    mat_cb_.data.uav_pick_flag_ = 0;
    memcpy(&res, ptr, sizeof(ShaderPickInfo));
    return res;
}

void RenderBase::beginMultTextureRender(const RenderColor& clear_color, int width, int height, bool is_clear_dep){
    if (!offscreen_texture_) {
        offscreen_texture_ = getRenderEngineInterface()->CreateTargetTextureBase();
    }
    if (!offscreen_texture_) {
        return;
    }

    offscreen_texture_->is_need_depth = false;
    offscreen_texture_->refreshTargetTexture(width, height);
    offscreen_texture_->clearColor(clear_color.Rgba());
    std::vector<RenderTargetTextureBase::BindSlotInfo> vec;
    RenderTargetTextureBase::BindSlotInfo t1;
    RenderTargetTextureBase::BindSlotInfo t2;
    t1.slot_ = 0;
    t2.slot_ = 1;
    t2.clear_tex_ = true;
    t2.bind_texture_ = offscreen_texture_.get();
    t1.clear_depth_ = is_clear_dep;
    t2.clear_depth_ = is_clear_dep;

    vec.push_back(t1);
    vec.push_back(t2);
    offscreen_texture_->setBindSlotsVec(vec, false);
}

void RenderBase::endMultTextureRender(){
    if (!offscreen_texture_) {
        return;
    }
    std::vector<RenderTargetTextureBase::BindSlotInfo> vec;
    offscreen_texture_->setBindSlotsVec({});
}

void RenderBase::renderModelBuffer(ModelBuffer* model_buf, RenderEngineInterface::DrawType type){}
void RenderBase::renderBlockBuffer(BlockBuffer* buf, RenderEngineInterface::DrawType type){}
void RenderBase::renderBlockBuffer(VertexBlockBuffer* buf, RenderEngineInterface::DrawType type){}

bool RenderBase::beginOffScreenRender(const RenderColor& clear_color, int width, int height){
    if (!offscreen_texture_) {
        offscreen_texture_ = getRenderEngineInterface()->CreateTargetTextureBase();
    }
    //每次进入begin都记录时间，超过一定时间没有进了进行删除
    //create_render_texture_time_ = base::GetTickTimeMS();
    if (!offscreen_texture_) {
        return false;
    }
    auto area_info = render_engine_->AreaInfo();
    if(width == 0 || height == 0) {
        width = area_info.win_w_;
        height = area_info.win_h_;
    }

    if (width <= 0 || height <= 0)
        return false;
    auto v = RenderEngineInterface::Viewport();
    v.x = 0;
    v.y = 0;
    v.w = width;
    v.h = height;
    last_view_port_ = render_context_->GetRenderEngineInterface()->GetLastViewport();
    render_context_->GetRenderEngineInterface()->SetViewport(v);
    offscreen_texture_->bindCurTexture(width, height, clear_color.Rgba(), 1, false);
    return true;
}

void RenderBase::endOffScreenRender(){
    if (!offscreen_texture_) {
        return;
    }
    render_context_->GetRenderEngineInterface()->SetViewport(last_view_port_);
    offscreen_texture_->setBindSlotsVec({});
}

//销毁长时间不使用的纹理 节省空间 销毁成功返回true
bool RenderBase::destoryLongTimeUnuseTexture() {
//    if (create_render_texture_time_ == 0) {
//        return false;
//    }
//    auto del_time = base::GetTickTimeMS() - create_render_texture_time_;
//    //30s不用就回收
//    if (del_time > 30 * 1000) {
//        this->offscreen_texture_.reset();
//        this->offscreen_texture_ = nullptr;
//        create_render_texture_time_ = 0;
//        return true;
//    }
//    return false;
    return true;
}
void RenderBase::fmat4x4Printf(const std::string& name, const geometry::DMatrix4x4& mat) {
    std::cout << "name = " << name << "__\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << *(mat.data() + j + i * 4) << ",";
        }
        std::cout << std::endl;
    }
}
}



#include "RenderEngineInterface.h"

namespace render {
void RenderEngineInterface::RenderModelBuffer(ModelBuffer* model_buf, DrawType type) const {
    for (auto& buf : model_buf->blockBuffers) {
        RenderBlockBuffer(buf.get(), type);
    }
}

std::shared_ptr<RenderProgramBase> RenderEngineInterface::CreateShaderProgramCB(const RenderProgramBase::ShaderInfo& info) const {
    auto res = CreateShaderProgram(info);
    return res;
}
struct RenderLogInfo {
    bool setRenderStatus_ = false;
    bool useShaderProgram_ = false;
    bool renderBlockBuffer_ = false;
    bool updataConstBuffer_ = false;
};

struct TickInfo
{
    int tick_begin_ = 0;
    int tick_end_ = 0;
};

std::unordered_map<RenderProgramBase*, RenderLogInfo> debug_info_map;
std::unordered_map<void*, std::unordered_map<std::string, TickInfo>> debug_info_map2;

#define DEBUG_LOG_INFO(name, bool_value)\
if (this->cur_program_) {\
    auto& info = debug_info_map[this->cur_program_];\
    if (!info.bool_value) {\
        std::cout << #name << "name: *******" << this->cur_program_->shader_info_.pragma_name_ << "*******" <<std::endl;\
        info.bool_value = true;\
    }\
}

#define DEBUG_LOG_INFO2(ptr, extra_info, bool_value, tick_count)\
if (ptr) {\
    auto& info = debug_info_map2[ptr];\
    auto& tick_info = info[extra_info];\
    if (tick_info.bool_value < tick_count) {\
        std::cout << #bool_value << "_____________info:_______________" << extra_info << std::endl;\
        tick_info.bool_value++;\
    }\
}

RenderEngineInterface::Viewport::Viewport(RenderContext* context) {
    int win_height = context->client_info_->windowInfo.GetWindowHeight();
    window_height = win_height;
}

void RenderEngineInterface::SetRenderStatusCB(const RenderStatusInfo& info) const {
    //DEBUG_LOG_INFO(SetRenderStatusCB, setRenderStatus_);
    SetRenderStatus(info);
}

void RenderEngineInterface::UseShaderProgramCB(std::shared_ptr<RenderProgramBase> c) const {
    //DEBUG_LOG_INFO(UseShaderProgramCB, useShaderProgram_);
    UseShaderProgram(c);
}

void RenderEngineInterface::RenderBlockBufferCB(BlockBuffer* buf, DrawType type) const {
    //DEBUG_LOG_INFO(RenderBlockBufferCB, renderBlockBuffer_);
    RenderBlockBuffer(buf, type);
}

void RenderEngineInterface::RenderBlockBufferCB(VertexBlockBuffer* buf, DrawType type) const {
    //DEBUG_LOG_INFO(RenderBlockBufferCB2, renderBlockBuffer_);
    RenderBlockBuffer(buf, type);
}

void RenderEngineInterface::RenderBlockBufferCB(RenderBufferBase* idx_buf, RenderBufferBase* v_buf, DrawType type,
    unsigned int index_size, unsigned index_offset) {
    //DEBUG_LOG_INFO(RenderBlockBufferCB3, renderBlockBuffer_);
    RenderBlockBuffer(idx_buf, v_buf, type, index_size, index_offset);
}

bool RenderEngineInterface::UpdataConstBufferCB(const std::shared_ptr<RenderConstBufferBase>& buffer) {
    //DEBUG_LOG_INFO(UpdataConstBufferCB, updataConstBuffer_);
    return UpdataConstBuffer(buffer);
}

void RenderEngineInterface::TickBegin(void* ptr, std::string extra_info, int tick_count) {
    DEBUG_LOG_INFO2(ptr, extra_info, tick_begin_, tick_count);
}

void RenderEngineInterface::TickEnd(void* ptr, std::string extra_info, int tick_count) {
    DEBUG_LOG_INFO2(ptr, extra_info, tick_end_, tick_count);
}

void RenderTargetTextureBase::bindCurTexture(int w, int h, const float* clear_color, int sample_level, bool is_display_screen) {
    this->cur_sample_level_ = sample_level;
    refreshTargetTexture(w, h);
    clearColor(clear_color);
    std::vector<RenderTargetTextureBase::BindSlotInfo> vec;
    RenderTargetTextureBase::BindSlotInfo t;
    t.slot_ = 0;
    t.bind_texture_ = this;
    t.is_dispay_srceen_render_ = is_display_screen;
    t.clear_tex_ = true;
    t.clear_depth_ = true;
    vec.push_back(t);
    setBindSlotsVec(vec);
}

framework::XImage RenderTargetTextureBase::getRenderImage(int start_x, int start_y, int width, int height) {
    auto oriImg = getRenderImage();
    if(start_x + width > tex_width
       || start_y + height > tex_height) {
          LOGE << "error startx = " << start_x << "read_wdith = " << width << "start_y = " << start_y << "read_height = " << height;
          return framework::XImage();
    }
    framework::XImage resImg(width, height, framework::XImage::RGBA);
    auto ori_data = oriImg.Data();
    auto res_data = resImg.Data();
    for(int y = start_y; y < start_y + resImg.Height(); y++) {
        for(int x = start_x; x < start_x + resImg.Width(); x++) {
            int ori_offset = (x + y * oriImg.Width()) * 4;
            int res_offset = (x - start_x + (y - start_y) * resImg.Width()) * 4;
            auto ori = ori_data + ori_offset;
            auto res = res_data + res_offset;
            memcpy(res, ori, 4);
        }
    }
    return resImg;
}

}  // namespace render

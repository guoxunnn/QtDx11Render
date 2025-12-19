#pragma once
#include <iostream>
#include <vector>
#include "../RenderData/RenderData.h"

namespace render {
class RenderConstBufferBase {
public:
    int data_size_ = 0;
    int slot_ = 0;
    std::string name_;
    void* data_ptr = nullptr;
};

template <class Type>
class RenderConstObject {
public:
    RenderConstObject() {
    }
    std::shared_ptr<RenderConstBufferBase> getBuffer() const {
        return buffer_;
    }
    void setBuffer(std::shared_ptr<RenderConstBufferBase> buf) {
        this->buffer_ = buf;
        buf->data_size_ = data_size_;
        buf->data_ptr = (void*)&data;
    }

public:
    int data_size_ = sizeof(Type);
    Type data;

private:
    std::shared_ptr<RenderConstBufferBase> buffer_ = nullptr;
};

class RenderProgramBase {
public:
    class ShaderInfo {
    public:
        struct MacroInfo {
            std::string name;
            std::string value;
        };
        std::string extra_info_;
        std::string pragma_name_;
        std::vector<MacroInfo> macros_vec_;
        enum InputType { VertexPos = 0, VertexPosNormal, VertexPosNormalTex , VertexPosColor};
        InputType input_type_ = VertexPos;
        bool gs_flag_ = true;
    };
    RenderProgramBase::ShaderInfo shader_info_;
};
struct RenderStatusInfo {
    enum DepthStencilType {
        Deafualt_Type = 0,
        NoDepth_Test_And_Write_Depth,
        NoDepth_Test_And_No_Write_Depth,
        Deafualt_Type_Write_Stencil,
        // 指定模板值才能通过渲染测试
        Deafualt_Type_Render_Assign_Stencil,
        Depth_Test_And_No_Write_Depth
    };
    enum DepthOffsetType {
        None = 0,
        //拉近摄像机距离
        Single,
        //双倍拉近摄像机距离
        Double,
        //远离摄像机距离
        Add
    };
    bool cull_mode = false;
    bool cull_back = true;
    bool transparent_blend = false;
    bool no_color_write_blend = false;
    // 模板值
    int stencil_value = 1;
    DepthStencilType depth_stencil_type = Deafualt_Type;
    DepthOffsetType depth_offset_type = DepthOffsetType::None;
    void reset() {
        cull_mode = false;
        cull_back = true;
        transparent_blend = false;
        no_color_write_blend = false;
        stencil_value = 1;
        depth_stencil_type = Deafualt_Type;
        depth_offset_type = DepthOffsetType::None;
    }
};

class SamplerStateBase {
public:
    struct SamplerInfo {
        enum WrapMode { Repeat, Mirrored_Repeat, Border };
        enum FilterType { Mipmap_Linerar, Linear };
        WrapMode wrap_mode_ = Repeat;
        FilterType filter_type = Mipmap_Linerar;
        int anisotrypy_level = -1;
        int minMapLoad = 0;
        int maxMapLoad = 1024;
    };
    SamplerInfo info_;
};
class RenderEngineInterface;
class RenderTargetTextureBase {
public:
    struct BindSlotInfo {
        int slot_ = 0;
        RenderTargetTextureBase* bind_texture_ = nullptr;
        bool clear_tex_ = false;
        bool clear_depth_ = false;
        //
        bool is_dispay_srceen_render_ = false;
    };
    RenderTargetTextureBase(const RenderEngineInterface* i) : render_interface_(i) {
    }
    virtual ~RenderTargetTextureBase() {
    }
    virtual void refreshTargetTexture(int w, int h) = 0;
    virtual void clearColor(const float* rgba) = 0;
    virtual void readPixel(int x, int y, unsigned char color[4]) = 0;
    virtual framework::XImage getRenderImage() = 0;
    virtual framework::XImage getRenderImage(int x, int y, int width, int height);
    virtual void bindDeafault() = 0;
    virtual void bindCurTexture(int w, int h, const float* clear_color, int sample_level,
                                bool is_display_scrren = false);
    virtual void setBindSlotsVec(const std::vector<BindSlotInfo>& v, bool is_swith_fbo_ = true, bool cache_flag = true) = 0;
    virtual void bindTexture2ShaderProgram(int slot, std::shared_ptr<SamplerStateBase> sampler_state,
                                           const std::string& tex_name = "") const = 0;
    virtual std::shared_ptr<RenderTextureBase> getTexture() = 0;

public:
    bool is_need_depth = true;
    unsigned int render_target_bind_slot_ = 0;
    const RenderEngineInterface* render_interface_;
    int tex_width = 0;
    int tex_height = 0;
    int cur_sample_level_ = 1;

protected:
    int last_sample_level_ = -1;
};
class RenderContext;
class RenderEngineInterface {
public:
    void SetBelongContext(RenderContext* context) {
        belong_conext_ = context;
    }
    enum DrawType { DrawPoints = 0, DrawLine, DrawLineStrip, DrawTriangle, DrawTriangleStrip};
    virtual void CreateGpuVertexBuffer(const float* pVertexes, size_t size,
                                       std::shared_ptr<RenderBufferBase>& pBuf) const = 0;
    virtual std::shared_ptr<RenderTextureBase> CreateTexutre(framework::XImage img, bool gen_mipmap = true) const {
        std::vector<framework::XImage> vec;
        vec.push_back(img);
        return CreateTexutre(vec, gen_mipmap);
    }
    virtual std::shared_ptr<RenderTextureBase> CreateTexutre(framework::XVector<framework::XImage> img_vec, bool gen_mipmap = true) const {
        std::vector<framework::XImage> img_v;
        for (int i = 0; i < img_vec.size(); i++) {
            img_v.push_back(img_vec[i]);
        }
        return CreateTexutre(img_v, gen_mipmap);
    }
    virtual std::shared_ptr<RenderTextureBase> CreateTexutre(std::vector<framework::XImage> img_vec, bool gen_mipmap = true) const {
        return nullptr;
    }
    virtual void CreateGpuIndexBuffer(const uint32_t* pIndices, size_t size,
                                      std::shared_ptr<RenderBufferBase>& pBuf) const = 0;
    virtual void CreateEmptyVertexBuffer(size_t size, std::shared_ptr<RenderBufferBase>& buf) {}
    virtual void CreateEmptyIndexBuffer(size_t size, std::shared_ptr<RenderBufferBase>& buf) {}
    // 不创建空间，覆盖原来的空间而已
    virtual void ModifyGpuBuffer(const void* pIndices, size_t size, std::shared_ptr<RenderBufferBase>& pBuf,
                                 size_t offset = 0) const = 0;
    virtual void CreateGpuBuffer(size_t size, std::shared_ptr<RenderBufferBase>& pBuf) {
    }
    virtual void DeleteGpuBuffer(std::shared_ptr<RenderBufferBase> pBuf) = 0;
    virtual std::shared_ptr<RenderConstBufferBase> CreateConstBuffer(int slot, int data_size,
                                                                     const std::string& var_name) const {
        return nullptr;
    }
    //void RenderModelBuffer(ModelBuffer* model_buf, DrawType type) const;
    virtual std::shared_ptr<RenderTargetTextureBase> CreateTargetTextureBase() const {
        return nullptr;
    }
    virtual void init() = 0;
    virtual void BindTexture(int slot, std::shared_ptr<RenderTextureBase> tex,
                             std::shared_ptr<SamplerStateBase> sampler_state, const std::string& tex_name = "") const {
    }
    virtual std::shared_ptr<SamplerStateBase> CreateSamplerState(const SamplerStateBase::SamplerInfo& info) const {
        return nullptr;
    }
    struct Viewport {
        Viewport(){}
        Viewport(int x1, int y1, int w1, int h1) {
            this->x = x1;
            this->y = y1;
            this->w = w1;
            this->h = h1;
        }
        Viewport(RenderContext* content);
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        int window_height = 0;
    };
    void SetViewport(const Viewport& viewport) {
        cur_viewport_ = viewport;
        if (viewport.window_height == 0) {
            cur_viewport_.window_height = viewport.h;
        }
        UpdataViewport();
    };
    Viewport GetLastViewport() const {
        return cur_viewport_;
    }
    RenderProgramBase* GetCurProgram() const {
        return cur_program_;
    }
    virtual void CopyResource(std::shared_ptr<RenderTargetTextureBase>& dest,
                              std::shared_ptr<RenderTargetTextureBase>& src) const {}
    virtual void CreatDetectSuspendBlendState() const {}
    virtual void UpdataViewport() const = 0;
    virtual void FrameStartOrEnd(bool is_start = true) {}
    virtual void ClearUav(std::shared_ptr<RenderUavBase> uav, int32_t value) const = 0;
    virtual std::shared_ptr<RenderUavBase> CreateUav(int count, int single_element_size, int slot, bool atomic_flag = false) const = 0;
    virtual void DestoryUav(std::shared_ptr<RenderUavBase> uav) = 0;
    virtual void* MapUav(std::shared_ptr<RenderUavBase> uav) = 0;
    virtual void UnMapUav(std::shared_ptr<RenderUavBase> uav) = 0;

    std::shared_ptr<RenderProgramBase> CreateShaderProgramCB(const RenderProgramBase::ShaderInfo& info) const;
    void SetRenderStatusCB(const RenderStatusInfo& info) const;
    void UseShaderProgramCB(std::shared_ptr<RenderProgramBase>) const;
    void RenderBlockBufferCB(BlockBuffer* buf, DrawType type) const;
    void RenderBlockBufferCB(VertexBlockBuffer* buf, DrawType type) const;
    void RenderBlockBufferCB(RenderBufferBase* idx_buf, RenderBufferBase* v_buf, DrawType type,
        unsigned int index_size, unsigned index_offset = 0);
    bool UpdataConstBufferCB(const std::shared_ptr<RenderConstBufferBase>& buffer);
    void TickBegin(void* ptr, std::string extra, int tick_count = 1);
    void TickEnd(void* ptr, std::string extra, int tick_count = 1);
    virtual void ClearCurDepth(){}
    virtual void ClearCurColor(const RenderColor& color){}
    virtual void BindUavViews(const std::vector<std::shared_ptr<RenderUavBase>>& vec) = 0;
    virtual void UnBindUavViews(const std::vector<std::shared_ptr<RenderUavBase>>& vec) = 0;

protected:
    virtual std::shared_ptr<RenderProgramBase> CreateShaderProgram(const RenderProgramBase::ShaderInfo& info) const {
        return nullptr;
    }
    virtual void SetRenderStatus(const RenderStatusInfo& info) const {
    }
    virtual void UseShaderProgram(std::shared_ptr<RenderProgramBase>) const {
    }
    virtual bool UpdataConstBuffer(const std::shared_ptr<RenderConstBufferBase>& buffer) const {
        return false;
    }
    virtual void RenderBlockBuffer(BlockBuffer* buf, DrawType type) const {}
    virtual void RenderBlockBuffer(VertexBlockBuffer* buf, DrawType type) const {}
    virtual void RenderBlockBuffer(RenderBufferBase* idx_buf, RenderBufferBase* v_buf, DrawType type,
        unsigned int index_size, unsigned index_offset = 0) const {}

public:
    RenderInterface render_interface_;
    RenderContext* belong_conext_ = nullptr;
    std::vector<RenderTargetTextureBase::BindSlotInfo> cur_bind_texture_vec_;
    RenderTargetTextureBase::BindSlotInfo dispay_srceen_render_bind_;

protected:
    bool is_init_flag = false;
    mutable RenderProgramBase* cur_program_ = nullptr;
    Viewport cur_viewport_;
};
}  // namespace render

#ifndef DX11RenderEngineInterface_H
#define DX11RenderEngineInterface_H

#include "../../manager/RenderEngineInterface.h"
#include "../RabbitDX11.h"
#include "../texture/Texture2D.h"

namespace render {
class DX11DataBuffer : RenderBufferBase {
public:
    ~DX11DataBuffer() {
        if (buf) buf->Release();
    }
    ID3D11Buffer* buf = nullptr;
};

class DX11TextureData : public RenderTextureBase {
public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tex = nullptr;
};

class DX11Uav : public RenderUavBase {
public:
    void SetInfo(int count, int single_element_size, int slot) {
        count_ = count;
        single_element_size_ = single_element_size;
        slot_ = slot;
    }

public:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buf = nullptr;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav = nullptr;
};

class DX11RenderConstBuffer : public RenderConstBufferBase {
public:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_ptr_ = nullptr;
};

class DX11RenderProgramBase : public RenderProgramBase {
public:
    // 顶点、像素、几何着色器
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_ = nullptr;
};

class DX11SamplerStateBase : public SamplerStateBase {
public:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_ = nullptr;
};

struct MyStatus {
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSWireframe = nullptr;  // 光栅化器状态：线框模式
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSNoCull = nullptr;     // 光栅化器状态：无背面裁剪模式
    // 深度偏移，比如两个面都是0.5 如果设置该状态，该渲染会将深度变为-0.45，这样可以避免深度竞争
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSNoCullDepthOffset = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSCullBackDepthOffset = nullptr;
    // 双倍偏移
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSNoCullDepthOffsetDouble =
        nullptr;  // 深度偏移，比如两个面都是0.5 如果设置该状态，该渲染会将深度变为-0.45，这样可以避免深度竞争

        // 双倍偏移
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSCullBackDepthOffsetDouble =
        nullptr;  // 深度偏移，比如两个面都是0.5 如果设置该状态，该渲染会将深度变为-0.45，这样可以避免深度竞争
                  // 深度偏移 不开启深度是-0.5 开启以后变为0.55 比原来更深，相当于深度竞争竞争不过别人
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSNoCullDepthOffsetAdd =
        nullptr;  // 深度偏移，比如两个面都是0.5 如果设置该状态，该渲染会将深度变为-0.45，这样可以避免深度竞争
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSCullClockWise = nullptr;  // 光栅化器状态：顺时针裁剪模式
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSCullAntiClockwise = nullptr;  // 光栅化器状态：逆时针裁剪模式

    Microsoft::WRL::ComPtr<ID3D11SamplerState> SSLinearWrap = nullptr;       // 采样器状态：线性过滤
    Microsoft::WRL::ComPtr<ID3D11SamplerState> SSAnisotropicWrap = nullptr;  // 采样器状态：各项异性过滤

    Microsoft::WRL::ComPtr<ID3D11BlendState> BSNoColorWrite = nullptr;     // 混合状态：不写入颜色
    Microsoft::WRL::ComPtr<ID3D11BlendState> BSTransparent = nullptr;      // 混合状态：透明混合
    Microsoft::WRL::ComPtr<ID3D11BlendState> BSAlphaToCoverage = nullptr;  // 混合状态：Alpha-To-Coverage
    Microsoft::WRL::ComPtr<ID3D11BlendState> BSAdditive = nullptr;         // 混合状态：加法混合
    Microsoft::WRL::ComPtr<ID3D11BlendState> BSDetectSuspend = nullptr;    // DetectSuspendModelRender

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSWriteStencil = nullptr;  // 深度/模板状态：写入模板值
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSDrawWithStencil =
        nullptr;  // 深度/模板状态：对指定模板值的区域进行绘制
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSNoDoubleBlend = nullptr;  // 深度/模板状态：无二次混合区域
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSNoDepthTest =
        nullptr;  // 深度/模板状态：关闭深度测试
                  // 深度/模板状态：关闭深度显示但是写入深度值
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSNoDepthTestWriteDepth = nullptr;
    // 深度/模板状态：仅深度测试，不写入深度值
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSNoDepthTestAndNoDepthWrite;
    //深度测试但是不写入深度
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSDepthTestAndNoDepthWrite = nullptr;
};

class DX11RenderTargetTextureBase : public RenderTargetTextureBase {
public:
    DX11RenderTargetTextureBase(const RenderEngineInterface* i) : RenderTargetTextureBase(i) {
    }
    virtual void refreshTargetTexture(int w, int h);
    virtual void clearColor(const float* rgba);
    virtual void readPixel(int x, int y, unsigned char color[4]);
    virtual framework::XImage getRenderImage();
    virtual framework::XImage getRenderImage(int x, int y, int width, int height);
    virtual void bindDeafault();
    virtual void setBindSlotsVec(const std::vector<BindSlotInfo>& v, bool is_swith_fbo_ = true, bool cache_flag = true);
    virtual void bindTexture2ShaderProgram(int slot, std::shared_ptr<SamplerStateBase> sampler_state,
                                           const std::string& tex_name = "") const;
    virtual std::shared_ptr<RenderTextureBase> getTexture();


private:
    std::shared_ptr<Depth2DMS> m_depth_buffer_ = nullptr;                        // 深度缓冲区
    std::shared_ptr<Texture2DMS> m_rtv_buffer_;                                  // G-Buffers
    std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> cache_rtv_vec_;  // 临时缓存的后备缓冲区
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cache_dsv_ = nullptr;  // 临时缓存的深度/模板缓冲区
    float clear_color_[4] = {0};
};

class DX11RenderEngineInterface : public RenderEngineInterface {
public:
    DX11RenderEngineInterface();

protected:
    virtual void init() override;
    void CreateGpuVertexBuffer(const float* pVertexes, size_t size,
                               std::shared_ptr<RenderBufferBase>& pBuf) const override;
    void CreateGpuIndexBuffer(const uint32_t* pIndices, size_t size,
                              std::shared_ptr<RenderBufferBase>& pBuf) const override;
    void ModifyGpuBuffer(const void* pIndices, size_t size, std::shared_ptr<RenderBufferBase>& pBuf,
                         size_t offset = 0) const override;
    void CreateEmptyVertexBuffer(size_t size, std::shared_ptr<RenderBufferBase>& buf);
    void CreateEmptyIndexBuffer(size_t size, std::shared_ptr<RenderBufferBase>& buf);

    void DeleteGpuBuffer(std::shared_ptr<RenderBufferBase> pBuf) override;
    virtual std::shared_ptr<RenderTextureBase> CreateTexutre(std::vector<framework::XImage> img_vec, bool gen_mipmap = true) const override;
    virtual std::shared_ptr<RenderConstBufferBase> CreateConstBuffer(int slot, int data_size,
                                                                     const std::string& var_name) const override;
    virtual bool UpdataConstBuffer(const std::shared_ptr<RenderConstBufferBase>& buffer) const override;
    virtual std::shared_ptr<RenderProgramBase> CreateShaderProgram(
        const RenderProgramBase::ShaderInfo& info) const override;
    virtual void UseShaderProgram(std::shared_ptr<RenderProgramBase>) const override;
    virtual void SetRenderStatus(const RenderStatusInfo& info) const override;
    virtual void RenderBlockBuffer(BlockBuffer* buf, DrawType type) const override;
    virtual void RenderBlockBuffer(VertexBlockBuffer* buf, DrawType type) const override;
    virtual void RenderBlockBuffer(RenderBufferBase* idx_buf, RenderBufferBase* v_buf, DrawType type,
                                   unsigned int index_size, unsigned index_offset = 0) const override;
    virtual std::shared_ptr<RenderTargetTextureBase> CreateTargetTextureBase() const;
    virtual void BindTexture(int slot, std::shared_ptr<RenderTextureBase> tex,
                             std::shared_ptr<SamplerStateBase> sampler_state,
                             const std::string& tex_name = "") const override;
    virtual std::shared_ptr<SamplerStateBase> CreateSamplerState(
        const SamplerStateBase::SamplerInfo& info) const override;
    virtual void CopyResource(std::shared_ptr<RenderTargetTextureBase>& dest,
                              std::shared_ptr<RenderTargetTextureBase>& src) const override;
    virtual void CreatDetectSuspendBlendState() const override;
    virtual void UpdataViewport() const override;
    virtual void ClearUav(std::shared_ptr<RenderUavBase> uav, int32_t value) const final;
    std::shared_ptr<RenderUavBase> CreateUav(int count, int single_element_size, int slot, bool atomic_flag = false) const final;
    void DestoryUav(std::shared_ptr<RenderUavBase> uav) final;
    void* MapUav(std::shared_ptr<RenderUavBase> uav) final;
    void BindUavViews(const std::vector<std::shared_ptr<RenderUavBase>>& vec) final;
    void UnBindUavViews(const std::vector<std::shared_ptr<RenderUavBase>>& vec) final;
    void UnMapUav(std::shared_ptr<RenderUavBase> uav) final;
    void ClearCurDepth();
    void ClearCurColor(const RenderColor& color);

private:
    void initRenderStatus();

private:
    std::shared_ptr<DX11RenderProgramBase> createRenderShader(const std::wstring& shader_dir,
                                                              const std::wstring& cso_shader_dir,
                                                              const RenderProgramBase::ShaderInfo& info) const;
    std::shared_ptr<MyStatus> status_ptr_ = nullptr;
};

};  // namespace render
#endif
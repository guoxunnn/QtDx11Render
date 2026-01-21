#include "DX11RenderEngineInterface.h"

#include "texture/Texture2D.h"
#include <map>
#include "base/log.h"

namespace render {
void DX11RenderTargetTextureBase::refreshTargetTexture(int w, int h) {
    if (w <= 0 || h <= 0) {
        std::cout << "!!!!!!!!!!!dx11 Texture error, tex w = " << w << " h = " << h;
        throw std::logic_error("w or h cant eq 0");
    }
    if (tex_height != h || tex_width != w || last_sample_level_ != cur_sample_level_) {
        ID3D11Device* device = (ID3D11Device*)render_interface_->render_interface_.drv;
        // ******************
        // 1. 创建纹理
        last_sample_level_ = cur_sample_level_;
        DXGI_SAMPLE_DESC sampleDesc;
        sampleDesc.Count = cur_sample_level_;
        sampleDesc.Quality = 0;
        if (is_need_depth) {
            // 不用抗锯齿可以使用添加模板 阴影渲染需要模板，所以需要8位的模板
            if (cur_sample_level_ == 1) {
                m_depth_buffer_ = std::make_shared<Depth2DMS>(
                    device, w, h, sampleDesc, DepthStencilBitsFlag::Depth_32Bits_Stencil_8Bits_Unused_24Bits);
            } else {
                m_depth_buffer_ =
                    std::make_shared<Depth2DMS>(device, w, h, sampleDesc, DepthStencilBitsFlag::Depth_32Bits);
            }
        }
        m_rtv_buffer_ = std::make_shared<Texture2DMS>(device, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, sampleDesc);
    }
    tex_height = h;
    tex_width = w;
    if (!m_rtv_buffer_) {
        LOGE << "m_rtv_buffer is error";
        throw std::logic_error("rtv is nullptr!!!");
    }
}

void DX11RenderTargetTextureBase::clearColor(const float* rgba) {
    clear_color_[0] = rgba[0];
    clear_color_[1] = rgba[1];
    clear_color_[2] = rgba[2];
    clear_color_[3] = rgba[3];
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_->render_interface_.drv_context;
    if(m_rtv_buffer_)
        context->ClearRenderTargetView(m_rtv_buffer_->GetRenderTarget(), clear_color_);
    else {
        std::vector<ID3D11RenderTargetView*> cache_rtv_vec(8);  // 临时缓存的后备缓冲区
        context->OMGetRenderTargets(cache_rtv_vec.size(), cache_rtv_vec.data(), cache_dsv_.ReleaseAndGetAddressOf());
        context->ClearRenderTargetView(cache_rtv_vec[0], clear_color_);
    }
}

void DX11RenderTargetTextureBase::readPixel(int x, int y, unsigned char color[4]) {
    Microsoft::WRL::ComPtr<ID3D11Resource> res;
    auto rtvs = m_rtv_buffer_->GetShaderResource();
    rtvs->GetResource(res.GetAddressOf());
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_->render_interface_.drv_context;
    DX11CommonFunc::DX11ReadPixels(context, res.Get(), x, y, color);
}

framework::XImage DX11RenderTargetTextureBase::getRenderImage() {
    framework::XImage img;
    img.Create(tex_width, tex_height, framework::XImage::RGBA);
    Microsoft::WRL::ComPtr<ID3D11Resource> res;
    auto rtvs = m_rtv_buffer_->GetShaderResource();
    rtvs->GetResource(res.GetAddressOf());
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_->render_interface_.drv_context;
    DX11CommonFunc::DX11ReadImage(context, res.Get(), 0, 0, tex_width, tex_height, (unsigned char*)img.Data());
    return img;
}

framework::XImage DX11RenderTargetTextureBase::getRenderImage(int x, int y, int width, int height) {
    framework::XImage img;
    img.Create(width, height, framework::XImage::RGBA);
    Microsoft::WRL::ComPtr<ID3D11Resource> res;
    auto rtvs = m_rtv_buffer_->GetShaderResource();
    rtvs->GetResource(res.GetAddressOf());
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_->render_interface_.drv_context;
    DX11CommonFunc::DX11ReadImage(context, res.Get(), x, y, width, height, (unsigned char*)img.Data());
    return img;
}

void DX11RenderTargetTextureBase::bindDeafault() {
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_->render_interface_.drv_context;
    if (cache_rtv_vec_.empty()) {
        return;
    }
    std::vector<ID3D11RenderTargetView*> cache_rtv_vec(cache_rtv_vec_.size());  // 临时缓存的后备缓冲区
    for (int i = 0; i < cache_rtv_vec_.size(); i++) {
        cache_rtv_vec[i] = cache_rtv_vec_[i].Get();
    }
    context->OMSetRenderTargets(cache_rtv_vec_.size(), cache_rtv_vec.data(), cache_dsv_.Get());
    for(auto& it : cache_rtv_vec_) {
        it.ReleaseAndGetAddressOf();
    }
    cache_rtv_vec_.clear();
}

void DX11RenderTargetTextureBase::bindTexture2ShaderProgram(int slot, std::shared_ptr<SamplerStateBase> sampler_state,
                                                            const std::string& tex_name) const {
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_->render_interface_.drv_context;
    if (!m_rtv_buffer_) {
        LOGE << "m_rtv_buffer is error";
        return;
    }
    auto tex = m_rtv_buffer_->GetShaderResource();
    context->PSSetShaderResources(slot, 1, &tex);
    if (sampler_state) {
        DX11SamplerStateBase* sam = (DX11SamplerStateBase*)sampler_state.get();
        context->PSSetSamplers(slot, 1, sam->sampler_state_.GetAddressOf());
    }
}

std::shared_ptr<RenderTextureBase> DX11RenderTargetTextureBase::getTexture() {
    auto shaderResourceView = m_rtv_buffer_->GetShaderResource();
    auto textureData = std::make_shared<DX11TextureData>();
    textureData->tex = shaderResourceView;
    return textureData;
}

void DX11RenderEngineInterface::BindUavViews(const std::vector<std::shared_ptr<RenderUavBase>>& vec) {
    std::map<int, ID3D11UnorderedAccessView*> sortCollect;
    for(auto& item : vec) {
        auto p = std::dynamic_pointer_cast<DX11Uav>(item);
        if(p && p->uav) {
            sortCollect[p->GetSlot()] = p->uav.Get();
        }
    }

    int startSlot = sortCollect.begin()->first;
    int endSlot = std::next(sortCollect.end(), -1)->first;

    std::vector<ID3D11UnorderedAccessView*> uavs;
    uavs.resize(endSlot - startSlot + 1);
    for(int i = 0; i < uavs.size(); ++i) {
        auto it = sortCollect.find(i + startSlot);
        if(it != sortCollect.end()) {
            uavs[i] = it->second;
        }else {
            uavs[i] = nullptr;
        }
    }

    render_interface_.drv_context->OMSetRenderTargetsAndUnorderedAccessViews(
        D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, startSlot, uavs.size(), &uavs[0], nullptr);
}

void DX11RenderEngineInterface::UnBindUavViews(const std::vector<std::shared_ptr<RenderUavBase>>& vec) {
    std::map<int, ID3D11UnorderedAccessView*> sortCollect;
    for(auto& item : vec) {
        auto p = std::dynamic_pointer_cast<DX11Uav>(item);
        if(p && p->uav) {
            sortCollect[p->GetSlot()] = p->uav.Get();
        }
    }

    int startSlot = sortCollect.begin()->first;
    int endSlot = std::next(sortCollect.end(), -1)->first;

    std::vector<ID3D11UnorderedAccessView*> uavs;
    uavs.resize(endSlot - startSlot + 1);
    for(auto& it : uavs) {
        it = nullptr;
    }

    render_interface_.drv_context->OMSetRenderTargetsAndUnorderedAccessViews(
        D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 1, uavs.size(), &uavs[0], nullptr);
}

void DX11RenderEngineInterface::CopyResource(std::shared_ptr<RenderTargetTextureBase>& dest,
                                             std::shared_ptr<RenderTargetTextureBase>& src) const {
    Microsoft::WRL::ComPtr<ID3D11Resource> s = nullptr, d = nullptr;
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    {
        DX11RenderTargetTextureBase* bind_data = (DX11RenderTargetTextureBase*)dest.get();
        if (!bind_data) {
            return;
        }
        auto bind_resource = bind_data->getTexture();
        auto tmp = (DX11TextureData*)bind_resource.get();
        if (!bind_resource) {
            return;
        }
        tmp->tex->GetResource(d.GetAddressOf());
    }

    {
        DX11RenderTargetTextureBase* bind_data = (DX11RenderTargetTextureBase*)src.get();
        auto bind_resource = bind_data->getTexture();
        auto tmp = (DX11TextureData*)bind_resource.get();
        tmp->tex->GetResource(s.GetAddressOf());
    }

    context->CopyResource(d.Get(), s.Get());
}

void DX11RenderEngineInterface::CreatDetectSuspendBlendState() const {
    ID3D11Device* device = (ID3D11Device*)render_interface_.drv;
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    auto& rtDesc = blendDesc.RenderTarget[0];
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = true;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0;
    blendDesc.RenderTarget[0].BlendEnable = false;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;         // 设置源混合
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;       // 设置目标混合
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;       // 设置混合操作
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;    // Alpha 源混合
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;  // Alpha 目标混合
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;  // Alpha 混合操作
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0;          // 不写入任何通道
    blendDesc.RenderTarget[1] = blendDesc.RenderTarget[0];
    blendDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    device->CreateBlendState(&blendDesc, status_ptr_->BSDetectSuspend.GetAddressOf());
}

void DX11RenderEngineInterface::UpdataViewport() const {
    D3D11_VIEWPORT screenViewport;
    screenViewport.TopLeftX = cur_viewport_.x;
    screenViewport.TopLeftY = cur_viewport_.y;
    screenViewport.Width = cur_viewport_.w;
    screenViewport.Height = cur_viewport_.h;
    screenViewport.MinDepth = 0.0f;
    screenViewport.MaxDepth = 1.0f;
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    context->RSSetViewports(1, &screenViewport);
}

std::shared_ptr<RenderUavBase> DX11RenderEngineInterface::CreateUav(int count, int single_element_size, int slot, bool atomic_flag) const {
    auto result = std::make_shared<DX11Uav>();
    result->SetInfo(count, single_element_size, slot);

    auto device = (ID3D11Device*)render_interface_.drv;

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferDesc.StructureByteStride = single_element_size;
    bufferDesc.ByteWidth = count * single_element_size;

    auto ok = device->CreateBuffer(&bufferDesc, nullptr, result->buf.ReleaseAndGetAddressOf());

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    uavDesc.Buffer.FirstElement = 0;
    if(atomic_flag)
        uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
    uavDesc.Buffer.NumElements = count;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    ok = device->CreateUnorderedAccessView(result->buf.Get(), &uavDesc, result->uav.ReleaseAndGetAddressOf());
    return result;
}

void DX11RenderEngineInterface::DestoryUav(std::shared_ptr<RenderUavBase> uav) {
    if(uav) {
        auto p = std::dynamic_pointer_cast<DX11Uav>(uav);
        p->buf.ReleaseAndGetAddressOf();
        p->uav.ReleaseAndGetAddressOf();
    }
}

void* DX11RenderEngineInterface::MapUav(std::shared_ptr<RenderUavBase> uav) {
    auto p = std::dynamic_pointer_cast<DX11Uav>(uav);
    if(p == nullptr || p->buf.Get() == nullptr) {
        return nullptr;
    }
    auto context = render_interface_.drv_context;
    D3D11_MAPPED_SUBRESOURCE map;

    context->Map(p->buf.Get(), 0, D3D11_MAP_READ, 0, &map);
    return map.pData;
}

void DX11RenderEngineInterface::ClearUav(std::shared_ptr<RenderUavBase> uav, int32_t value) const {
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    DX11Uav* uav_ptr = (DX11Uav*)uav.get();

    {
        UINT initialCount = 0; // 设置计数器的初始值
        context->CSSetUnorderedAccessViews(
            0,                  // StartSlot
            1,                  // NumUAVs
            uav_ptr->uav.GetAddressOf(),           // UAV 视图
            &initialCount       // 初始计数器值
        );
        ID3D11UnorderedAccessView* nullUAV = nullptr;
        context->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
    }

    {
        ID3D11UnorderedAccessView* pUAV = uav_ptr->uav.Get(); // 假设这是你的 UAV 对象
        UINT clearValue[4] = { 0, 0, 0, 0 }; // 设置清空值为 0
        context->ClearUnorderedAccessViewUint(pUAV, clearValue);
    }

    //D3D11_MAPPED_SUBRESOURCE map;
    //context->Map(uav_ptr->buf.Get(), 0, D3D11_MAP_WRITE, 0, &map);
    //memset(map.pData, 0, uav->GetCount() * uav->GetSingleElementSize());
    //render_interface_.drv_context->Unmap(uav_ptr->buf.Get(), 0);
}

void DX11RenderEngineInterface::UnMapUav(std::shared_ptr<RenderUavBase> uav) {
    auto p = std::dynamic_pointer_cast<DX11Uav>(uav);
    if(p == nullptr || p->buf.Get() == nullptr) {
        return ;
    }

    render_interface_.drv_context->Unmap(p->buf.Get(), 0);
}


void DX11RenderEngineInterface::ClearCurDepth(){
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
    render_interface_.drv_context->OMGetRenderTargets(1, nullptr, pDepthStencilView.GetAddressOf());
    render_interface_.drv_context->ClearDepthStencilView(pDepthStencilView.Get(),
                                                         D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DX11RenderEngineInterface::ClearCurColor(const RenderColor& input_color) {
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pCacheRTV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pCacheDSV;
    const float* color_float = input_color.Rgba();
    auto context = render_interface_.drv_context;
    // 获取刚刚设置的缓冲区
    context->OMGetRenderTargets(1, pCacheRTV.GetAddressOf(), pCacheDSV.GetAddressOf());
    context->ClearRenderTargetView(pCacheRTV.Get(), color_float);
    context->ClearDepthStencilView(pCacheDSV.Get(),
                                      D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetDepthStencilState(nullptr, 0);
}

void DX11RenderTargetTextureBase::setBindSlotsVec(const std::vector<BindSlotInfo>& vec, bool is_swith_fbo, bool cache_flag) {
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_->render_interface_.drv_context;
    // vec就用上一次的
    if (vec.size() == 0) {
        std::vector<ID3D11RenderTargetView*> cache_rtv_vec(cache_rtv_vec_.size());  // 临时缓存的后备缓冲区
        if (cache_rtv_vec_.empty()) {
            return;
        }
        for (int i = 0; i < cache_rtv_vec_.size(); i++) {
            cache_rtv_vec[i] = cache_rtv_vec_[i].Get();
        }
        context->OMSetRenderTargets(cache_rtv_vec_.size(), cache_rtv_vec.data(), cache_dsv_.Get());
        render_interface_->UpdataViewport();
        return;
    }
    std::vector<ID3D11RenderTargetView*> rtv_vec;  // 临时缓存的后备缓冲区
    if(cache_flag){
        for(auto& it : cache_rtv_vec_) {
            it.ReleaseAndGetAddressOf();
        }
        cache_rtv_vec_.clear();
        std::vector<ID3D11RenderTargetView*> cache_rtv_vec(8);  // 临时缓存的后备缓冲区
        cache_rtv_vec_.resize(cache_rtv_vec.size());
        context->OMGetRenderTargets(cache_rtv_vec.size(), cache_rtv_vec.data(), cache_dsv_.ReleaseAndGetAddressOf());
        for (int i = 0; i < 8; i++) {
            cache_rtv_vec_[i].Attach(cache_rtv_vec[i]);
        }
    }
    ID3D11DepthStencilView* dsv = nullptr;  // 临时缓存的深度/模板缓冲区
    bool is_clear_depth = false;
    for (int i = 0; i < vec.size(); i++) {
        auto slot = vec[i].slot_;
        DX11RenderTargetTextureBase* texture = (DX11RenderTargetTextureBase*)vec[i].bind_texture_;
        if (texture) {
            if (texture->is_need_depth) {
                dsv = texture->m_depth_buffer_->GetDepthStencil();
            }
            rtv_vec.push_back(texture->m_rtv_buffer_->GetRenderTarget());
        } else {
            if (i < cache_rtv_vec_.size()) {
                rtv_vec.push_back(cache_rtv_vec_[i].Get());
            }
        }
        if (vec[i].clear_depth_) {
            is_clear_depth = true;
        }
    }
    ID3D11DepthStencilView* cur_depth = nullptr;
    if (dsv) {
        cur_depth = dsv;
        context->OMSetRenderTargets(rtv_vec.size(), rtv_vec.data(), dsv);
    } else {
        cur_depth = cache_dsv_.Get();
        context->OMSetRenderTargets(rtv_vec.size(), rtv_vec.data(), cache_dsv_.Get());
    }
    if (is_clear_depth) {
        context->ClearDepthStencilView(cur_depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
    render_interface_->UpdataViewport();
}

DX11RenderEngineInterface::DX11RenderEngineInterface() {
}

void DX11RenderEngineInterface::init() {
    if (is_init_flag) {
        return;
    }
    initRenderStatus();
    is_init_flag = true;
}

void DX11RenderEngineInterface::CreateGpuVertexBuffer(const float* pVertexes, size_t size,
                                                      std::shared_ptr<RenderBufferBase>& buf) const {
    if (size == 0) {
        return;
    }
    auto dx11_buff = new DX11DataBuffer;
    buf.reset((RenderBufferBase*)dx11_buff);
    DX11DataBuffer* b = (DX11DataBuffer*)buf.get();
    auto device = (ID3D11Device*)render_interface_.drv;
    b->buf = D3DUtil::CreateDX11Buffer(device, static_cast<const void*>(pVertexes), sizeof(float) * size,
                                       D3D11_BIND_VERTEX_BUFFER);
}

void DX11RenderEngineInterface::CreateEmptyVertexBuffer(size_t size, std::shared_ptr<RenderBufferBase>& buf) {
    if (size == 0) {
        return;
    }
    auto dx11_buff = new DX11DataBuffer;
    buf.reset((RenderBufferBase*)dx11_buff);
    DX11DataBuffer* b = (DX11DataBuffer*)buf.get();
    auto device = (ID3D11Device*)render_interface_.drv;
    b->buf = D3DUtil::CreateDX11Buffer(device, size, D3D11_BIND_VERTEX_BUFFER);
}

void DX11RenderEngineInterface::CreateEmptyIndexBuffer(size_t size, std::shared_ptr<RenderBufferBase>& buf) {
    auto dx11_buff = new DX11DataBuffer;
    buf.reset((RenderBufferBase*)dx11_buff);
    DX11DataBuffer* b = (DX11DataBuffer*)buf.get();
    auto device = (ID3D11Device*)render_interface_.drv;
    b->buf = D3DUtil::CreateDX11Buffer(device, size, D3D11_BIND_INDEX_BUFFER);
}

void DX11RenderEngineInterface::CreateGpuIndexBuffer(const uint32_t* pIndices, size_t size,
                                                     std::shared_ptr<RenderBufferBase>& buf) const {
    auto dx11_buff = new DX11DataBuffer;
    buf.reset((RenderBufferBase*)dx11_buff);
    DX11DataBuffer* b = (DX11DataBuffer*)buf.get();
    auto device = (ID3D11Device*)render_interface_.drv;
    b->buf = D3DUtil::CreateDX11Buffer(device, static_cast<const void*>(pIndices), sizeof(uint32_t) * size,
                                       D3D11_BIND_INDEX_BUFFER);
}

void DX11RenderEngineInterface::ModifyGpuBuffer(const void* pIndices, size_t size,
                                                std::shared_ptr<RenderBufferBase>& pBuf, size_t offset) const {
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    DX11DataBuffer* b = (DX11DataBuffer*)pBuf.get();
    ID3D11Buffer* d3d_index_buf = b->buf;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HR(context->Map(d3d_index_buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
    void* d = (void*)mappedResource.pData;
    memcpy((char*)(d) + offset, (void*)pIndices, size);
    context->Unmap(d3d_index_buf, 0);
}

std::shared_ptr<RenderTextureBase> DX11RenderEngineInterface::CreateTexutre(
    std::vector<framework::XImage> img_vec, bool gen_mipmap) const {
    if (img_vec.size() == 0) {
        return nullptr;
    }
    int w = img_vec[0].Width();
    int h = img_vec[0].Height();
    if (w == 0 || h == 0) {
        return nullptr;
    }
    auto img_format = img_vec[0].PixelFormat();
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    int pix_byte = 4;
    if (img_format == framework::XImage::GRAY) {
        format = DXGI_FORMAT_R8_UNORM;
        pix_byte = 1;
    }
    auto ptr = std::make_shared<DX11TextureData>();
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    if (img_vec.size() > 1) {
        srvDesc.Texture2D.MipLevels = -1;
    } else {
        srvDesc.Texture2D.MipLevels = 1;
    }
    srvDesc.Texture2D.MostDetailedMip = 0;

    std::vector<D3D11_SUBRESOURCE_DATA> subDataVec(img_vec.size());
    for (int i = 0; i < img_vec.size(); i++) {
        subDataVec[i].pSysMem = img_vec[i].Data();
        int width = img_vec[i].Width();
        subDataVec[i].SysMemPitch = width * pix_byte;
    }

    D3D11_TEXTURE2D_DESC texDesc2;
    texDesc2.Width = img_vec[0].Width();
    texDesc2.Height = img_vec[0].Height();
    texDesc2.MipLevels = subDataVec.size();
    texDesc2.ArraySize = 1;
    texDesc2.Format = format;
    texDesc2.SampleDesc.Count = 1;  // ��ʹ�ö��ز���
    texDesc2.SampleDesc.Quality = 0;
    texDesc2.Usage = D3D11_USAGE_DEFAULT;
    texDesc2.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc2.CPUAccessFlags = 0;
    texDesc2.MiscFlags = 0;  // ָ����Ҫ����mipmap
    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2;
    auto device = (ID3D11Device*)render_interface_.drv;
    auto hr = device->CreateTexture2D(&texDesc2, subDataVec.data(), tex2.GetAddressOf());

    auto hr2 = device->CreateShaderResourceView(tex2.Get(), &srvDesc, ptr->tex.GetAddressOf());
    return ptr;
}

std::shared_ptr<RenderConstBufferBase> DX11RenderEngineInterface::CreateConstBuffer(int slot, int data_size,
                                                                                    const std::string& var_name) const {
    auto ptr = std::make_shared<DX11RenderConstBuffer>();
    D3D11_BUFFER_DESC cbd;
    ZeroMemory(&cbd, sizeof(cbd));
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.ByteWidth = data_size;
    ptr->slot_ = slot;
    ptr->name_ = var_name;
    ptr->data_size_ = data_size;
    auto device = (ID3D11Device*)render_interface_.drv;
    HRESULT hres = device->CreateBuffer(&cbd, nullptr, ptr->buffer_ptr_.GetAddressOf());
    return ptr;
}

bool DX11RenderEngineInterface::UpdataConstBuffer(const std::shared_ptr<RenderConstBufferBase>& buffer) const {
    DX11RenderConstBuffer* ptr = (DX11RenderConstBuffer*)buffer.get();
    D3D11_MAPPED_SUBRESOURCE mappedData;
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    context->Map(ptr->buffer_ptr_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    memcpy_s(mappedData.pData, ptr->data_size_, ptr->data_ptr, ptr->data_size_);
    context->Unmap(ptr->buffer_ptr_.Get(), 0);
    context->VSSetConstantBuffers(ptr->slot_, 1, ptr->buffer_ptr_.GetAddressOf());
    context->GSSetConstantBuffers(ptr->slot_, 1, ptr->buffer_ptr_.GetAddressOf());
    context->PSSetConstantBuffers(ptr->slot_, 1, ptr->buffer_ptr_.GetAddressOf());
    return true;
}

std::shared_ptr<DX11RenderProgramBase> DX11RenderEngineInterface::createRenderShader(
    const std::wstring& shader_dir, const std::wstring& cso_shader_dir, const RenderProgramBase::ShaderInfo& info) const {
    std::wstring vs_path = shader_dir + L"vs.hlsl";
    std::wstring gs_path = shader_dir + L"gs.hlsl";
    std::wstring ps_path = shader_dir + L"ps.hlsl";
    std::wstring vs_path_cso = cso_shader_dir + L"vs.cso";
    std::wstring gs_path_cso = cso_shader_dir + L"gs.cso";
    std::wstring ps_path_cso = cso_shader_dir + L"ps.cso";
    const WCHAR* vs_cso = nullptr;
    const WCHAR* gs_cso = nullptr;
    const WCHAR* ps_cso = nullptr;

    if (cso_shader_dir.size() > 0) {
        vs_cso = vs_path_cso.c_str();
        gs_cso = gs_path_cso.c_str();
        ps_cso = ps_path_cso.c_str();
    }
    std::vector<D3D_SHADER_MACRO> defines;
    for (auto it = info.macros_vec_.begin(); it != info.macros_vec_.end(); it++) {
        D3D_SHADER_MACRO m;
        m.Name = it->name.c_str();
        m.Definition = it->value.c_str();
        defines.push_back(m);
    }
    D3D_SHADER_MACRO m;
    m.Name = nullptr;
    m.Definition = nullptr;
    defines.push_back(m);
    auto device = (ID3D11Device*)render_interface_.drv;
    std::shared_ptr<DX11RenderProgramBase> shader_ptr = std::make_shared<DX11RenderProgramBase>();

    Microsoft::WRL::ComPtr<ID3DBlob> blob;
    HRESULT result;
    // vs
    {
        result = D3DUtil::CreateShaderFromFile(vs_cso, vs_path.c_str(), "VS", "vs_5_0", blob.GetAddressOf(), defines.data(), defines.size());
        ERROR_CHECK(result == S_OK) << "CreateShaderFromFile failed. result:" << result;
        result = device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr,
                                            shader_ptr->vs_.GetAddressOf());
        ERROR_CHECK(result == S_OK) << "CreateVertexShader failed. result:" << result;

        ERROR_CHECK(result == S_OK) << "CreateInputLayout failed. result:" << result;
    } 

    // input
    {
        if (info.input_type_ == RenderProgramBase::ShaderInfo::VertexPos) {
            result = device->CreateInputLayout(VertexPos::inputLayout, ARRAYSIZE(VertexPos::inputLayout),
                                               blob->GetBufferPointer(), blob->GetBufferSize(),
                                               shader_ptr->input_layout_.GetAddressOf());
        } else if (info.input_type_ == RenderProgramBase::ShaderInfo::VertexPosNormalTex) {
            result = device->CreateInputLayout(VertexPosNormalTex::inputLayout,
                                               ARRAYSIZE(VertexPosNormalTex::inputLayout), blob->GetBufferPointer(),
                                               blob->GetBufferSize(), shader_ptr->input_layout_.GetAddressOf());
        }else if(info.input_type_ == RenderProgramBase::ShaderInfo::VertexPosColor) {
                        result = device->CreateInputLayout(VertexPosColor::inputLayout,
                                               ARRAYSIZE(VertexPosColor::inputLayout), blob->GetBufferPointer(),
                                               blob->GetBufferSize(), shader_ptr->input_layout_.GetAddressOf());
        }
    }

    // gs
    {
        result = D3DUtil::CreateShaderFromFile(ps_cso, ps_path.c_str(), "PS", "ps_5_0", blob.ReleaseAndGetAddressOf(),
                                               defines.data(), defines.size()); 
        ERROR_CHECK(result == S_OK) << "CreateShaderFromFile2 failed. result:" << result;

        result = device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr,
                                            shader_ptr->ps_.GetAddressOf());
        ERROR_CHECK(result == S_OK) << "CreatePixelShader failed. result:" << result;
    }   

    // gs
    if (info.gs_flag_) {
        result = D3DUtil::CreateShaderFromFile(gs_cso, gs_path.c_str(), "GS", "gs_5_0", blob.ReleaseAndGetAddressOf(),
                                               defines.data(), defines.size());
        ERROR_CHECK(result == S_OK) << "CreateShaderFromFile failed. result:" << result;
        result = device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr,
                                              shader_ptr->gs_.GetAddressOf());
        ERROR_CHECK(result == S_OK) << "CreateGeometryShader failed. result:" << result;
    }

    return shader_ptr;
}

std::shared_ptr<RenderProgramBase> DX11RenderEngineInterface::CreateShaderProgram(
    const RenderProgramBase::ShaderInfo& info) const {

    std::string dir = "resources/shader/DX11Shader2/" + info.pragma_name_ + "/";
    std::string cso_dir = "";
    auto w_dir = D3DUtil::GetWC(dir.c_str(), dir.size());
    std::wstring cso_w_dir;
    if(cso_dir.size())
        cso_w_dir = D3DUtil::GetWC(cso_dir.c_str(), cso_dir.size());
    auto res = createRenderShader(w_dir, cso_w_dir, info);
    res->shader_info_ = info;
    return res;
}

void DX11RenderEngineInterface::UseShaderProgram(std::shared_ptr<RenderProgramBase> buf) const {
    DX11RenderProgramBase* program = (DX11RenderProgramBase*)buf.get();
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    context->VSSetShader(program->vs_.Get(), nullptr, 0);
    context->GSSetShader(program->gs_.Get(), nullptr, 0);
    context->PSSetShader(program->ps_.Get(), nullptr, 0);
    context->IASetInputLayout(program->input_layout_.Get());
    cur_program_ = buf.get();
}
void DX11RenderEngineInterface::SetRenderStatus(const RenderStatusInfo& info) const {
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    if(info.depth_offset_type == RenderStatusInfo::DepthOffsetType::None){
        if (!info.cull_mode) {
            context->RSSetState(status_ptr_->RSNoCull.Get());
        } else {
            if (info.cull_back) {
                context->RSSetState(status_ptr_->RSCullClockWise.Get());
            } else {
                context->RSSetState(status_ptr_->RSCullAntiClockwise.Get());
            }
        }
    }else{
        if(info.depth_offset_type == RenderStatusInfo::DepthOffsetType::Single){
            if (info.cull_mode && info.cull_back) {
                context->RSSetState(status_ptr_->RSCullBackDepthOffset.Get());
            }
            else {
                context->RSSetState(status_ptr_->RSNoCullDepthOffset.Get());
            }
        }else if(info.depth_offset_type == RenderStatusInfo::DepthOffsetType::Double){
            if (info.cull_mode && info.cull_back) {
                context->RSSetState(status_ptr_->RSCullBackDepthOffsetDouble.Get());
            }
            else {
                context->RSSetState(status_ptr_->RSNoCullDepthOffsetDouble.Get());
            }
        }else if(info.depth_offset_type == RenderStatusInfo::DepthOffsetType::Add){
            context->RSSetState(status_ptr_->RSNoCullDepthOffsetAdd.Get());
        }
    }

    if (info.no_color_write_blend) {
        context->OMSetBlendState(status_ptr_->BSNoColorWrite.Get(), nullptr, 0xFFFFFFFF);
    } else if (info.transparent_blend) {
        context->OMSetBlendState(status_ptr_->BSTransparent.Get(), nullptr, 0xFFFFFFFF);
    } else {
        context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    }

    if (info.depth_stencil_type == RenderStatusInfo::NoDepth_Test_And_Write_Depth) {
        context->OMSetDepthStencilState(status_ptr_->DSSNoDepthTestWriteDepth.Get(), 0x0);
    } else if (info.depth_stencil_type == RenderStatusInfo::NoDepth_Test_And_No_Write_Depth) {
        context->OMSetDepthStencilState(status_ptr_->DSSNoDepthTestAndNoDepthWrite.Get(), 0x0);
    } else if (info.depth_stencil_type == RenderStatusInfo::Deafualt_Type_Write_Stencil) {
        context->OMSetDepthStencilState(status_ptr_->DSSWriteStencil.Get(), info.stencil_value);
    } else if (info.depth_stencil_type == RenderStatusInfo::Deafualt_Type_Render_Assign_Stencil) {
        context->OMSetDepthStencilState(status_ptr_->DSSDrawWithStencil.Get(), info.stencil_value);
    }
    else if (info.depth_stencil_type == RenderStatusInfo::Depth_Test_And_No_Write_Depth) {
        context->OMSetDepthStencilState(status_ptr_->DSSDepthTestAndNoDepthWrite.Get(), 0);
    }
    else {
        context->OMSetDepthStencilState(nullptr, 0x0);
    }
}

D3D11_PRIMITIVE_TOPOLOGY getGLRenderMode(DX11RenderEngineInterface::DrawType type) {
    D3D11_PRIMITIVE_TOPOLOGY mode = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    if (type == DX11RenderEngineInterface::DrawTriangle) {
        mode = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    } else if (type == DX11RenderEngineInterface::DrawLine) {
        mode = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    } else if (type == DX11RenderEngineInterface::DrawTriangleStrip) {
        mode = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    } else if (type == DX11RenderEngineInterface::DrawLineStrip) {
        mode = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    } else if (type == DX11RenderEngineInterface::DrawPoints) {
        mode = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    }
    return mode;
}

void DX11RenderEngineInterface::RenderBlockBuffer(BlockBuffer* buf, DrawType type) const {
    if (!buf || !buf->vertex_handle_) {
        return;
    }
    UINT offset = 0;
    UINT stride = sizeof(geometry::FVector3D);
    if (cur_program_) {
        if (cur_program_->shader_info_.input_type_ == RenderProgramBase::ShaderInfo::VertexPosNormalTex) {
            stride = sizeof(geometry::FVector3D) + sizeof(geometry::FVector3D) + sizeof(geometry::FVector2D);
        } else if (cur_program_->shader_info_.input_type_ == RenderProgramBase::ShaderInfo::VertexPosNormal) {
            stride = sizeof(geometry::FVector3D) + sizeof(geometry::FVector3D);
        }
    }
    DX11DataBuffer* v_buf = (DX11DataBuffer*)buf->vertex_handle_.get();
    DX11DataBuffer* i_buf = (DX11DataBuffer*)buf->index_handle_.get();
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    context->IASetVertexBuffers(0, 1, (ID3D11Buffer* const*)&v_buf->buf, &stride, &offset);
    context->IASetPrimitiveTopology(getGLRenderMode(type));

    if (buf->index_handle_) {
        context->IASetIndexBuffer(i_buf->buf, DXGI_FORMAT_R32_UINT, 0);
        if (buf->index_size_ > 0) {
            context->DrawIndexed(buf->index_size_, 0, 0);
        }
    }
}

void DX11RenderEngineInterface::RenderBlockBuffer(VertexBlockBuffer* buf, DrawType type) const {
    if (buf->vertex_handle_ == nullptr) {
        return;
    }
    UINT offset = 0;
    UINT stride = sizeof(geometry::FVector3D);
    auto input_type = this->cur_program_->shader_info_.input_type_;
    if (input_type == RenderProgramBase::ShaderInfo::VertexPosColor) {
        stride = sizeof(geometry::FVector3D) + sizeof(geometry::FVector3D);
    }
    else if (input_type == RenderProgramBase::ShaderInfo::VertexPosNormalTex) {
        stride = sizeof(geometry::FVector3D) + sizeof(geometry::FVector3D) + sizeof(geometry::FVector2D);
    }
    DX11DataBuffer* v_buf = (DX11DataBuffer*)buf->vertex_handle_.get();
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    context->IASetVertexBuffers(0, 1, (ID3D11Buffer* const*)&v_buf->buf, &stride, &offset);
    context->IASetPrimitiveTopology(getGLRenderMode(type));
    context->Draw(buf->vertex_size, 0);
}

void DX11RenderEngineInterface::RenderBlockBuffer(RenderBufferBase* i, RenderBufferBase* v, DrawType type,
                                                  unsigned int index_size, unsigned index_offset) const {
    if (!i || !v) {
        return;
    }
    UINT offset = 0;
    UINT stride = sizeof(geometry::FVector3D);
    if (cur_program_) {
        if (cur_program_->shader_info_.input_type_ == RenderProgramBase::ShaderInfo::VertexPosNormalTex) {
            stride = sizeof(geometry::FVector3D) + sizeof(geometry::FVector3D) + sizeof(geometry::FVector2D);
        } else if (cur_program_->shader_info_.input_type_ == RenderProgramBase::ShaderInfo::VertexPosNormal) {
            stride = sizeof(geometry::FVector3D) + sizeof(geometry::FVector3D);
        }
    }
    DX11DataBuffer* v_buf = (DX11DataBuffer*)v;
    DX11DataBuffer* i_buf = (DX11DataBuffer*)i;
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    context->IASetVertexBuffers(0, 1, (ID3D11Buffer* const*)&v_buf->buf, &stride, &offset);
    context->IASetPrimitiveTopology(getGLRenderMode(type));

    context->IASetIndexBuffer(i_buf->buf, DXGI_FORMAT_R32_UINT, 0);
    context->DrawIndexed(index_size, index_offset, 0);
}

std::shared_ptr<RenderTargetTextureBase> DX11RenderEngineInterface::CreateTargetTextureBase() const {
    return std::make_shared<DX11RenderTargetTextureBase>(this);
}

void DX11RenderEngineInterface::BindTexture(int slot, std::shared_ptr<RenderTextureBase> tex,
                                            std::shared_ptr<SamplerStateBase> sampler_state,
                                            const std::string& tex_name) const {
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)render_interface_.drv_context;
    DX11TextureData* t = (DX11TextureData*)tex.get();
    if (t) context->PSSetShaderResources(slot, 1, t->tex.GetAddressOf());
    if (sampler_state) {
        DX11SamplerStateBase* sam = (DX11SamplerStateBase*)sampler_state.get();
        context->PSSetSamplers(0, 1, sam->sampler_state_.GetAddressOf());
    }
}

std::shared_ptr<SamplerStateBase> DX11RenderEngineInterface::CreateSamplerState(
    const SamplerStateBase::SamplerInfo& info) const {
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    if (info.filter_type == SamplerStateBase::SamplerInfo::FilterType::Linear) {
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    } else if (info.filter_type == SamplerStateBase::SamplerInfo::FilterType::Mipmap_Linerar) {
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    }
    if (info.anisotrypy_level > 0) {
        sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        sampDesc.MaxAnisotropy = info.anisotrypy_level;
    }
    D3D11_TEXTURE_ADDRESS_MODE wrap_mode;
    if (info.wrap_mode_ == SamplerStateBase::SamplerInfo::WrapMode::Mirrored_Repeat) {
        wrap_mode = D3D11_TEXTURE_ADDRESS_MIRROR;
    }
    else if (info.wrap_mode_ == SamplerStateBase::SamplerInfo::WrapMode::Border) {
        wrap_mode = D3D11_TEXTURE_ADDRESS_BORDER;
    }
    else {
        wrap_mode = D3D11_TEXTURE_ADDRESS_WRAP;
    }
    sampDesc.AddressU = wrap_mode;
    sampDesc.AddressV = wrap_mode;
    sampDesc.AddressW = wrap_mode;
    sampDesc.MinLOD = info.minMapLoad;
    sampDesc.MaxLOD = info.maxMapLoad;
    auto ptr = std::make_shared<DX11SamplerStateBase>();
    ID3D11Device* device = (ID3D11Device*)render_interface_.drv;
    device->CreateSamplerState(&sampDesc, ptr->sampler_state_.GetAddressOf());
    return ptr;
}

void DX11RenderEngineInterface::initRenderStatus() {
    if (status_ptr_) {
        return;
    }
    status_ptr_ = std::make_shared<MyStatus>();
    ID3D11Device* device = (ID3D11Device*)render_interface_.drv;
    // ******************
    // ��ʼ����դ����״̬
    //
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
    // �߿�ģʽ
    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthClipEnable = true;
    device->CreateRasterizerState(&rasterizerDesc, status_ptr_->RSWireframe.GetAddressOf());

    // �ޱ����޳�ģʽ
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = false;
    device->CreateRasterizerState(&rasterizerDesc, status_ptr_->RSNoCull.GetAddressOf());
    {
        auto rst_desc = rasterizerDesc;
        rst_desc.DepthClipEnable = true;
        // if (DepthBiasClamp > 0)
        //     Bias = min(DepthBiasClamp, Bias)
        // else if (DepthBiasClamp < 0)
        //     Bias = max(DepthBiasClamp, Bias)
        // ��������Ȳ���֮ǰ�����ֵƫ��һ���̶�ֵ�������������Z-fighting����ȳ�ͻ���� ��ֵ��ʹ����Զ���������ֵ��ʹ���忿�����
        // DepthBias ��һ������ƫ��ֵ����������ȱȽ�֮ǰ��֮������ֵ���е�������ֵ������ֵ����ֵ���㣬ͨ�����ӻ�������ֵ�������Ⱦ������⡣DepthBias
        // �����������ʩ����ͬ��ƫ��ֵ�������ڴ����򵥵����ƫ�����
        rst_desc.DepthBias = -10;
        // �����������ƫ�Ƶ����ֵ����Сֵ�����������С��ƫ��ֵ���²���Ҫ��Ч����
        rst_desc.DepthBiasClamp = -0.05;
        /*SlopeScaledDepthBias ��ֵָ�������ƫ�������ű��淨�ߵı仯���仯�����ʡ��������Ϊ
        0�����ʾ������б�ʵ�Ӱ�죬���ƫ�Ƶı仯���ʽ������ķ����޹ء�
        ������б��������ƫ��ʱ���������Ӹ�ֵ��ʹ�ö�б������ƫ�Ƹ������У��Ӷ���������Ⱦʱ�������ƫ����������⡣*/
        rst_desc.SlopeScaledDepthBias = 0;
        device->CreateRasterizerState(&rst_desc, status_ptr_->RSNoCullDepthOffset.GetAddressOf());
        rst_desc.CullMode = D3D11_CULL_BACK;
        device->CreateRasterizerState(&rst_desc, status_ptr_->RSCullBackDepthOffset.GetAddressOf());
        rst_desc.CullMode = D3D11_CULL_NONE;
        rst_desc.DepthBias = -100;
        rst_desc.DepthBiasClamp = -0.1;
        device->CreateRasterizerState(&rst_desc, status_ptr_->RSNoCullDepthOffsetDouble.GetAddressOf());
        rst_desc.CullMode = D3D11_CULL_BACK;
        device->CreateRasterizerState(&rst_desc, status_ptr_->RSCullBackDepthOffsetDouble.GetAddressOf());
        rst_desc.CullMode = D3D11_CULL_NONE;
    }
    {
        auto rst_desc = rasterizerDesc;
        rst_desc.DepthClipEnable = true;
        rst_desc.DepthBias = 10;
        rst_desc.DepthBiasClamp = 0.05;
        rst_desc.SlopeScaledDepthBias = 0;
        device->CreateRasterizerState(&rst_desc, status_ptr_->RSNoCullDepthOffsetAdd.GetAddressOf());
    }

    // ˳ʱ���޳�ģʽ
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = true;
    device->CreateRasterizerState(&rasterizerDesc, status_ptr_->RSCullClockWise.GetAddressOf());
    // ˳ʱ���޳�ģʽ
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_FRONT;
    rasterizerDesc.FrontCounterClockwise = true;
    device->CreateRasterizerState(&rasterizerDesc, status_ptr_->RSCullAntiClockwise.GetAddressOf());

    // ******************
    // ��ʼ��������״̬
    //
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));

    // ���Թ���ģʽ
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, status_ptr_->SSLinearWrap.GetAddressOf());

    // �������Թ���ģʽ
    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MaxAnisotropy = 4;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, status_ptr_->SSAnisotropicWrap.GetAddressOf());

    // ******************
    // ��ʼ�����״̬
    //
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    auto& rtDesc = blendDesc.RenderTarget[0];
    // Alpha-To-Coverageģʽ
    blendDesc.AlphaToCoverageEnable = true;
    blendDesc.IndependentBlendEnable = false;
    rtDesc.BlendEnable = false;
    rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    device->CreateBlendState(&blendDesc, status_ptr_->BSAlphaToCoverage.GetAddressOf());

    // ͸�����ģʽ
    // RenderColor = SrcAlpha * SrcColor + (1 - SrcAlpha) * DestColor
    // Alpha = SrcAlpha
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = true;
    rtDesc.BlendEnable = true;
    rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[1] = rtDesc;
    blendDesc.RenderTarget[1].BlendEnable = false;
    device->CreateBlendState(&blendDesc, status_ptr_->BSTransparent.GetAddressOf());

    // �ӷ����ģʽ
    // RenderColor = SrcColor + DestColor
    // Alpha = SrcAlpha
    rtDesc.SrcBlend = D3D11_BLEND_ONE;
    rtDesc.DestBlend = D3D11_BLEND_ONE;
    rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

    device->CreateBlendState(&blendDesc, status_ptr_->BSAdditive.GetAddressOf());

    // ����ɫд����ģʽ
    // RenderColor = DestColor
    // Alpha = DestAlpha
    rtDesc.BlendEnable = false;
    rtDesc.SrcBlend = D3D11_BLEND_ZERO;
    rtDesc.DestBlend = D3D11_BLEND_ONE;
    rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
    rtDesc.DestBlendAlpha = D3D11_BLEND_ONE;
    rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask = 0;
    device->CreateBlendState(&blendDesc, status_ptr_->BSNoColorWrite.GetAddressOf());

    // ******************
    // ��ʼ�����/ģ��״̬
    //
    D3D11_DEPTH_STENCIL_DESC dsDesc;

    // д��ģ��ֵ�����/ģ��״̬
    // ���ﲻд�������Ϣ
    // ���������滹�Ǳ��棬ԭ��ָ���������ģ��ֵ���ᱻд��StencilRef
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // ���ڱ���ļ����������ǲ�������Ⱦ�ģ���������������޹ؽ�Ҫ
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSWriteStencil.GetAddressOf());

    // ��ָ��ģ��ֵ���л��Ƶ����/ģ��״̬
    // ������ģ��ֵ����������Ž��л��ƣ����������
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    // ���ڱ���ļ����������ǲ�������Ⱦ�ģ���������������޹ؽ�Ҫ
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSDrawWithStencil.GetAddressOf());

    // �޶��λ�����/ģ��״̬
    // ����Ĭ����Ȳ���
    // ͨ���Ե���ʹ��ԭ��StencilRef��ֵֻ��ʹ��һ�Σ�ʵ�ֽ�һ�λ��
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    // ���ڱ���ļ����������ǲ�������Ⱦ�ģ���������������޹ؽ�Ҫ
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSNoDoubleBlend.GetAddressOf());

    // �ر���Ȳ��Ե����/ģ��״̬
    // �����Ʒ�͸�����壬����ϸ��ջ���˳��
    // ����͸����������Ҫ���Ļ���˳��
    // ��Ĭ�������ģ����Ծ��ǹرյ�
    dsDesc.DepthEnable = false;
    dsDesc.StencilEnable = false;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSNoDepthTest.GetAddressOf());

    dsDesc.DepthEnable = true;
    dsDesc.StencilEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    (device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSNoDepthTestWriteDepth.GetAddressOf()));

    // ������Ȳ��ԣ�����д�����ֵ��״̬
    // �����Ʒ�͸������ʱ��Ӧʹ��Ĭ��״̬
    // ����͸������ʱ��ʹ�ø�״̬������Чȷ�����״̬�Ľ���
    // ����ȷ����ǰ�ķ�͸����������赲�Ϻ��һ������
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.StencilEnable = false;

    (device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSNoDepthTestAndNoDepthWrite.GetAddressOf()));
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    (device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSDepthTestAndNoDepthWrite.GetAddressOf()));
    // ******************
    // ���õ��Զ�����
    //
    // D3D11SetDebugObjectName(RSCullClockWise.Get(), "RSCullClockWise");
    // D3D11SetDebugObjectName(RSNoCull.Get(), "RSNoCull");
    // D3D11SetDebugObjectName(RSWireframe.Get(), "RSWireframe");
    // D3D11SetDebugObjectName(RSCullAntiClockwise.Get(), "RSCullAntiClockwise");

    // D3D11SetDebugObjectName(SSAnisotropicWrap.Get(), "SSAnisotropicWrap");
    // D3D11SetDebugObjectName(SSLinearWrap.Get(), "SSLinearWrap");

    // D3D11SetDebugObjectName(BSAlphaToCoverage.Get(), "BSAlphaToCoverage");
    // D3D11SetDebugObjectName(BSNoColorWrite.Get(), "BSNoColorWrite");
    // D3D11SetDebugObjectName(BSTransparent.Get(), "BSTransparent");
    // D3D11SetDebugObjectName(BSAdditive.Get(), "BSAdditive");

    // D3D11SetDebugObjectName(DSSWriteStencil.Get(), "DSSWriteStencil");
    // D3D11SetDebugObjectName(DSSDrawWithStencil.Get(), "DSSDrawWithStencil");
    // D3D11SetDebugObjectName(DSSNoDoubleBlend.Get(), "DSSNoDoubleBlend");
    // D3D11SetDebugObjectName(DSSNoDepthTest.Get(), "DSSNoDepthTest");
    // D3D11SetDebugObjectName(DSSNoDepthTestAndNoDepthWrite.Get(), "DSSNoDepthTestAndNoDepthWrite");
}

void DX11RenderEngineInterface::DeleteGpuBuffer(std::shared_ptr<RenderBufferBase> pBuf) {
    DX11DataBuffer* b = (DX11DataBuffer*)pBuf.get();
    if (b && b->buf) {
        b->buf->Release();
        b->buf = nullptr;
    }
}

};  // namespace render

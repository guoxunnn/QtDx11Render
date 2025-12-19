#include "DX11CommonFunc.h"

#include "d3d11.h"
#include "d3dcompiler.h"

DXGI_FORMAT EnsureNotTypeless(DXGI_FORMAT fmt) noexcept {
    // Assumes UNORM or FLOAT; doesn't use UINT or SINT
    switch (fmt) {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case DXGI_FORMAT_R32G32B32_TYPELESS: return DXGI_FORMAT_R32G32B32_FLOAT;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_UNORM;
        case DXGI_FORMAT_R32G32_TYPELESS: return DXGI_FORMAT_R32G32_FLOAT;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS: return DXGI_FORMAT_R10G10B10A2_UNORM;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_R16G16_TYPELESS: return DXGI_FORMAT_R16G16_UNORM;
        case DXGI_FORMAT_R32_TYPELESS: return DXGI_FORMAT_R32_FLOAT;
        case DXGI_FORMAT_R8G8_TYPELESS: return DXGI_FORMAT_R8G8_UNORM;
        case DXGI_FORMAT_R16_TYPELESS: return DXGI_FORMAT_R16_UNORM;
        case DXGI_FORMAT_R8_TYPELESS: return DXGI_FORMAT_R8_UNORM;
        case DXGI_FORMAT_BC1_TYPELESS: return DXGI_FORMAT_BC1_UNORM;
        case DXGI_FORMAT_BC2_TYPELESS: return DXGI_FORMAT_BC2_UNORM;
        case DXGI_FORMAT_BC3_TYPELESS: return DXGI_FORMAT_BC3_UNORM;
        case DXGI_FORMAT_BC4_TYPELESS: return DXGI_FORMAT_BC4_UNORM;
        case DXGI_FORMAT_BC5_TYPELESS: return DXGI_FORMAT_BC5_UNORM;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS: return DXGI_FORMAT_B8G8R8X8_UNORM;
        case DXGI_FORMAT_BC7_TYPELESS: return DXGI_FORMAT_BC7_UNORM;
        default: return fmt;
    }
}
/**
 * @brief CaptureTexture
 * @param pContext 上下文
 * @param pSource  纹理资源
 * @param desc     纹理描述
 * @param pStaging 拷贝的坐标点将放在这里面
 * @param x 点击的坐标X
 * @param y 点击的坐标Y
 * @param size 抓取图像区域大小
 * @return
 */
HRESULT CaptureTexture(_In_ ID3D11DeviceContext* pContext, _In_ ID3D11Resource* pSource,
                       D3D11_TEXTURE2D_DESC& desc, Microsoft::WRL::ComPtr<ID3D11Texture2D>& pStaging, int x,
                       int y, int width = 2, int height = 2) noexcept {
    if (!pContext || !pSource) return E_INVALIDARG;

    D3D11_RESOURCE_DIMENSION resType = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    pSource->GetType(&resType);

    if (resType != D3D11_RESOURCE_DIMENSION_TEXTURE2D) return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
    // 获取到纹理并存储起来
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

    // 0x6f15aaf2,0xd208,0x4e89,0x9a,0xb4,0x48,0x95,0x35,0xd3,0x4f,0x9c
    GUID id;
    id.Data1 = 0x6f15aaf2;
    id.Data2 = 0xd208;
    id.Data3 = 0x4e89;
    id.Data4[0] = 0x9a;
    id.Data4[1] = 0xb4;
    id.Data4[2] = 0x48;
    id.Data4[3] = 0x95;
    id.Data4[4] = 0x35;
    id.Data4[5] = 0xd3;
    id.Data4[6] = 0x4f;
    id.Data4[7] = 0x9c;

    // HRESULT hr = pSource->QueryInterface(IID_ID3D11Texture2D,
    // reinterpret_cast<void**>(pTexture.GetAddressOf()));
    HRESULT hr = pSource->QueryInterface(id, reinterpret_cast<void**>(pTexture.GetAddressOf()));

    if (FAILED(hr)) return hr;

    assert(pTexture);
    // 获取资源的描述
    pTexture->GetDesc(&desc);
    // 不能大于纹理点长和宽
    if (desc.Width < x || desc.Height < y) { return E_INVALIDARG; }

    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;
    pContext->GetDevice(d3dDevice.GetAddressOf());
    // 设置模板资源的大小
    desc.Width = width;
    desc.Height = height;
    // 设置源资源要读的区域
    D3D11_BOX sourceRegion;
    sourceRegion.left = x;
    sourceRegion.right = x + desc.Width;
    sourceRegion.top = y;
    sourceRegion.bottom = y + desc.Height;
    sourceRegion.front = 0;
    sourceRegion.back = 1;

    if (desc.SampleDesc.Count > 1) {
        // MSAA content must be resolved before being copied to a staging texture
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> pTemp;
        hr = d3dDevice->CreateTexture2D(&desc, nullptr, pTemp.GetAddressOf());
        if (FAILED(hr)) return hr;

        assert(pTemp);

        const DXGI_FORMAT fmt = EnsureNotTypeless(desc.Format);

        UINT support = 0;
        hr = d3dDevice->CheckFormatSupport(fmt, &support);
        if (FAILED(hr)) return hr;

        if (!(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE)) return E_FAIL;

        for (UINT item = 0; item < desc.ArraySize; ++item) {
            for (UINT level = 0; level < desc.MipLevels; ++level) {
                const UINT index = D3D11CalcSubresource(level, item, desc.MipLevels);
                pContext->ResolveSubresource(pTemp.Get(), index, pSource, index, fmt);
            }
        }

        desc.BindFlags = 0;
        desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;
        hr = d3dDevice->CreateTexture2D(&desc, nullptr, pStaging.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;

        assert(pStaging);
        pContext->CopySubresourceRegion(pStaging.Get(), 0, 0, 0, 0, pTemp.Get(), 0, &sourceRegion);
    } else if ((desc.Usage == D3D11_USAGE_STAGING) && (desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ)) {
        // Handle case where the source is already a staging texture we can use directly
        pStaging = pTexture;
    } else {
        // Otherwise, create a staging texture from the non-MSAA source
        desc.BindFlags = 0;
        desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;

        hr = d3dDevice->CreateTexture2D(&desc, nullptr, pStaging.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;

        assert(pStaging);
        pContext->CopySubresourceRegion(pStaging.Get(), 0, 0, 0, 0, pSource, 0, &sourceRegion);
    }

    return S_OK;
}

// HRESULT DX11CommonFunc::SaveDDSTextureToFile(ID3D11DeviceContext* pContext, ID3D11Resource* pSource,
//                                              const wchar_t* fileName) {
//     return DirectX::SaveDDSTextureToFile(pContext, pSource, fileName);
// }

void DX11CommonFunc::DX11ReadPixels(ID3D11DeviceContext* pContext, ID3D11Resource* pSource, int pX, int pY,
                                    uint8_t pColor[4]) {
    D3D11_TEXTURE2D_DESC desc = {};
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pStaging;
    if (FAILED(CaptureTexture(pContext, pSource, desc, pStaging, pX, pY))) { return; }
    D3D11_MAPPED_SUBRESOURCE mapped;
    pContext->Map(pStaging.Get(), 0, D3D11_MAP_READ, 0, &mapped);
    auto sptr = static_cast<const uint8_t*>(mapped.pData);
    if(sptr)
        memcpy_s(&pColor[0], 4, sptr, 4);
    pContext->Unmap(pStaging.Get(), 0);
}

void DX11CommonFunc::DX11ReadImage(ID3D11DeviceContext* pContext, ID3D11Resource* pSource, uint32_t pX,
                                   uint32_t pY, uint32_t width, uint32_t height, unsigned char* pColor) {
    D3D11_TEXTURE2D_DESC desc = {};
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pStaging;
    if (FAILED(CaptureTexture(pContext, pSource, desc, pStaging, pX, pY, width, height))) { return; }
    D3D11_MAPPED_SUBRESOURCE mapped;
    pContext->Map(pStaging.Get(), 0, D3D11_MAP_READ, 0, &mapped);
    // 一行的数据量
    int rowPitch = mapped.RowPitch;
    uint32_t rowSize = width * 4;
    for (int i = 0; i < height; i++) {
        auto sptr = static_cast<const unsigned char*>((mapped.pData));
        memcpy_s(pColor + rowSize * i, rowSize, sptr + rowPitch * i, rowSize);
    }
    pContext->Unmap(pStaging.Get(), 0);
}

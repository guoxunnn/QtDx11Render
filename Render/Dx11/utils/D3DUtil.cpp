#include "D3DUtil.h"

#include <cstdio>
#include <cwchar>
#include <string>
#include "base/log.h"

namespace render {
namespace D3DUtil {
// ------------------------------
// CreateShaderFromFile函数
// ------------------------------
// [In]csoFileNameInOut 编译好的着色器二进制文件(.cso)，若有指定则优先寻找该文件并读取
// [In]hlslFileName     着色器代码，若未找到着色器二进制文件则编译着色器代码
// [In]entryPoint       入口点(指定开始的函数)
// [In]shaderModel      着色器模型，格式为"*s_5_0"，*可以为c,d,g,h,p,v之一
// [Out]ppBlobOut       输出着色器二进制信息
HRESULT WINAPI DXTraceW(_In_z_ const WCHAR* strFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ const WCHAR* strMsg,
                        _In_ bool bPopMsgBox) {
    WCHAR strBufferFile[MAX_PATH];
    WCHAR strBufferLine[128];
    WCHAR strBufferError[300];
    WCHAR strBufferMsg[1024];
    WCHAR strBufferHR[40];
    WCHAR strBuffer[3000];

    swprintf_s(strBufferLine, 128, L"%lu", dwLine);
    if (strFile) {
        swprintf_s(strBuffer, 3000, L"%ls(%ls): ", strFile, strBufferLine);
        OutputDebugStringW(strBuffer);
    }

    size_t nMsgLen = (strMsg) ? wcsnlen_s(strMsg, 1024) : 0;
    if (nMsgLen > 0) {
        OutputDebugStringW(strMsg);
        OutputDebugStringW(L" ");
    }
    // Windows SDK 8.0起DirectX的错误信息已经集成进错误码中，可以通过FormatMessageW获取错误信息字符串
    // 不需要分配字符串内存
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), strBufferError, 256, nullptr);

    WCHAR* errorStr = wcsrchr(strBufferError, L'\r');
    if (errorStr) {
        errorStr[0] = L'\0';  // 擦除FormatMessageW带来的换行符(把\r\n的\r置换为\0即可)
    }

    swprintf_s(strBufferHR, 40, L" (0x%0.8x)", hr);
    wcscat_s(strBufferError, strBufferHR);
    swprintf_s(strBuffer, 3000, L"error：%ls", strBufferError);
    OutputDebugStringW(strBuffer);

    OutputDebugStringW(L"\n");

//    if (bPopMsgBox) {
//        wcscpy_s(strBufferFile, MAX_PATH, L"");
//        if (strFile) wcscpy_s(strBufferFile, MAX_PATH, strFile);

//        wcscpy_s(strBufferMsg, 1024, L"");
//        if (nMsgLen > 0) swprintf_s(strBufferMsg, 1024, L"current use：%ls\n", strMsg);

//        swprintf_s(strBuffer, 3000, L"filename：%ls\nline：%ls\nerror：%ls\n%ls you need debug？",
//                   strBufferFile, strBufferLine, strBufferError, strBufferMsg);

//        int nResult = MessageBoxW(GetForegroundWindow(), strBuffer, L"error", MB_YESNO | MB_ICONERROR);
//        if (nResult == IDYES) DebugBreak();
//    }

    return hr;
}

std::wstring GetWC(const WCHAR* str) {
    std::wstring result;
    result.resize(std::wcslen(str));
    memcpy(result.data(), str, result.size() * sizeof(WCHAR));
    return result;
}

std::wstring GetWC(const char* str, int size) {
    std::wstring result;
    result.resize(size);
    mbstowcs(result.data(), str, result.size());
    return GetWC(result.data());
}

// ------------------------------
// CreateShaderFromFile函数
// ------------------------------
// [In]csoFileNameInOut 编译好的着色器二进制文件(.cso)，若有指定则优先寻找该文件并读取
// [In]hlslFileName     着色器代码，若未找到着色器二进制文件则编译着色器代码
// [In]entryPoint       入口点(指定开始的函数)
// [In]shaderModel      着色器模型，格式为"*s_5_0"，*可以为c,d,g,h,p,v之一
// [Out]ppBlobOut       输出着色器二进制信息
HRESULT CreateShaderFromFile(const WCHAR* csoFileNameInOut, const WCHAR* hlslFileName, LPCSTR entryPoint,
                             LPCSTR shaderModel, ID3DBlob** ppBlobOut, const D3D_SHADER_MACRO* pDefines, int define_size) {
    HRESULT hr = S_OK;
    auto debug_flag = false;

    // 寻找是否有已经编译好的顶点着色器
    if (define_size <= 1 && !debug_flag && csoFileNameInOut && D3DReadFileToBlob(csoFileNameInOut, ppBlobOut) == S_OK) {
        return hr;
    } else {
        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
        if (debug_flag){
            // 设置 D3DCOMPILE_DEBUG 标志用于获取着色器调试信息。该标志可以提升调试体验，
            // 但仍然允许着色器进行优化操作
            dwShaderFlags |= D3DCOMPILE_DEBUG;

            // 在Debug环境下禁用优化以避免出现一些不合理的情况
            dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
        }

        ID3DBlob* errorBlob = nullptr;
        auto pathStr =  GetWC(hlslFileName);
        hr = D3DCompileFromFile(pathStr.data(), pDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
                                dwShaderFlags, 0, ppBlobOut, &errorBlob);
        if (FAILED(hr)) {
            LOGE << "D3DCompileFromFile failed. result:" << hr;
            if (errorBlob != nullptr) {
                OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
                std::string str = (char*)errorBlob->GetBufferPointer();
                LOGE << "errorBlob: " << str << "\n";
            }
            SAFE_RELEASE(errorBlob);
            return hr;
        }

        // 若指定了输出文件名，则将着色器二进制信息输出
        if (csoFileNameInOut && define_size < 2) {
            return D3DWriteBlobToFile(*ppBlobOut, csoFileNameInOut, FALSE);
        }
    }

    return hr;
}

ID3D11Buffer* CreateDX11Buffer(ID3D11Device* pDevice, int size, D3D11_BIND_FLAG type) {
    ID3D11Buffer* buffer = nullptr;
    D3D11_BUFFER_DESC ibd;
    ibd.ByteWidth = size;
    ibd.BindFlags = type;
    // GPU (只读) 和 CPU (只写) 访问的资源。 对于每帧由 CPU 至少更新一次的资源，动态资源是一个不错的选择。
    // 若要更新动态资源，请使用 Map 方法。
    ibd.Usage = D3D11_USAGE_DYNAMIC;
    ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ibd.MiscFlags = 0;
    HRESULT hr;
    hr = pDevice->CreateBuffer(&ibd, nullptr, &buffer);
    ERROR_CHECK(hr == S_OK) << "CreateBuffer failed. result:" << hr;
    hr = pDevice->GetDeviceRemovedReason();
    ERROR_CHECK(hr == S_OK) << "GetDeviceRemovedReason failed. result:" << hr;
    return buffer;
}

ID3D11Buffer* CreateDX11Buffer(ID3D11Device* pDevice, const void* data, int size, D3D11_BIND_FLAG type) {
    ID3D11Buffer* buffer = nullptr;
    D3D11_BUFFER_DESC ibd;
    ibd.ByteWidth = size;
    ibd.BindFlags = type;
    // GPU (只读) 和 CPU (只写) 访问的资源。 对于每帧由 CPU 至少更新一次的资源，动态资源是一个不错的选择。
    // 若要更新动态资源，请使用 Map 方法。
    ibd.Usage = D3D11_USAGE_DYNAMIC;
    ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = data;
    HRESULT hr;
    hr = pDevice->CreateBuffer(&ibd, &iinitData, &buffer);
    ERROR_CHECK(hr == S_OK) << "CreateBuffer failed. result:" << hr;
    hr = pDevice->GetDeviceRemovedReason();
    ERROR_CHECK(hr == S_OK) << "GetDeviceRemovedReason failed. result:" << hr;
    return buffer;
}

void CreateDX11Buffer(ID3D11Device* pDevice, Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer, const void* data, int size,
                      D3D11_BIND_FLAG type) {
    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = size;
    ibd.BindFlags = type;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = data;
    auto hr = pDevice->CreateBuffer(&ibd, &iinitData, buffer.GetAddressOf());
    ERROR_CHECK(hr == S_OK) << "CreateBuffer failed. result:" << hr;
}

DirectX::FXMMATRIX DMat2D11Mat(const geometry::DMatrix4x4& d_max) {
    float dat[16];
    for (size_t u = 0; u < 16; ++u) {
        dat[u] = d_max.constData()[u];
    }
    return DirectX::FXMMATRIX(dat);
}
}  // namespace D3DUtil
}  // namespace render

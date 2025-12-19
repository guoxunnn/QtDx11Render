#ifndef d3dutil_h
#define d3dutil_h

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <filesystem>
#include <iostream>
#include <DirectXMath.h>

#include "geometry/DMatrix4x4.h"

namespace render {

// 安全COM组件释放宏
#define SAFE_RELEASE(p)     \
    {                       \
        if ((p)) {          \
            (p)->Release(); \
            (p) = nullptr;  \
        }                   \
    }


// ------------------------------
// DXTraceW函数
// ------------------------------
// 在调试输出窗口中输出格式化错误信息，可选的错误窗口弹出(已汉化)
// [In]strFile			当前文件名，通常传递宏__FILEW__
// [In]hlslFileName     当前行号，通常传递宏__LINE__
// [In]hr				函数执行出现问题时返回的HRESULT值
// [In]strMsg			用于帮助调试定位的字符串，通常传递L#x(可能为NULL)
// [In]bPopMsgBox       如果为TRUE，则弹出一个消息弹窗告知错误信息
// 返回值: 形参hr

namespace D3DUtil {
    std::wstring GetWC(const char* str, int size);
    HRESULT WINAPI DXTraceW(_In_z_ const WCHAR* strFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ const WCHAR* strMsg, _In_ bool bPopMsgBox);
    HRESULT CreateShaderFromFile(const WCHAR* csoFileNameInOut, const WCHAR* hlslFileName,
                                LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut,
                                const D3D_SHADER_MACRO* pDefines = nullptr, int define_size = 0);
    ID3D11Buffer* CreateDX11Buffer(ID3D11Device* pDevice, const void* data, int size, D3D11_BIND_FLAG type = D3D11_BIND_VERTEX_BUFFER);
    ID3D11Buffer* CreateDX11Buffer(ID3D11Device* pDevice, int size, D3D11_BIND_FLAG type);
    void CreateDX11Buffer(ID3D11Device* pDevice, Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer, const void* data, int size, D3D11_BIND_FLAG type = D3D11_BIND_VERTEX_BUFFER);

    DirectX::FXMMATRIX DMat2D11Mat(const geometry::DMatrix4x4& d_max);
    
};
#define HR(x)												\
{															\
	HRESULT hr = (x);										\
	if(FAILED(hr))											\
    {											\
		D3DUtil::DXTraceW(__FILEW__, (DWORD)__LINE__, hr, L#x, true);\
	}														\
}
}  // namespace render
#endif


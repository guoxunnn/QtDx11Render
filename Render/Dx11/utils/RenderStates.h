//***************************************************************************************
// RenderStates.h by X_Jun(MKXJun) (C) 2018-2022 All Rights Reserved.
// Licensed under the MIT License.
//
// 提供一些渲染状态
// Provide some render states.
//***************************************************************************************

#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "d3d11.h"
#include "d3dcompiler.h"
#include <wrl.h>
#include <memory>

namespace render {

class RenderStates {
public:
    static void initAll(ID3D11Device * device);
    static void destory();

public:
    struct MyStatus
    {
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSWireframe = nullptr;		            // 光栅化器状态：线框模式
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSNoCull = nullptr;			            // 光栅化器状态：无背面裁剪模式
        //有深度偏移
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSNoCullDepthOffset = nullptr;			// 深度偏移，比如两个面都是0.5 如果设置该状态，该渲染会将深度变为-0.45，这样可以避免深度竞争
        //双倍偏移
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSNoCullDepthOffsetDouble = nullptr;			// 深度偏移，比如两个面都是0.5 如果设置该状态，该渲染会将深度变为-0.45，这样可以避免深度竞争
                                                                                            //深度偏移 不开启深度是-0.5 开启以后变为0.55 比原来更深，相当于深度竞争竞争不过别人
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSNoCullDepthOffsetAdd = nullptr;			// 深度偏移，比如两个面都是0.5 如果设置该状态，该渲染会将深度变为-0.45，这样可以避免深度竞争
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSCullClockWise = nullptr;	            // 光栅化器状态：顺时针裁剪模式
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSCullAntiClockwise = nullptr;           // 光栅化器状态：逆时针裁剪模式

        Microsoft::WRL::ComPtr<ID3D11SamplerState> SSLinearWrap = nullptr;			            // 采样器状态：线性过滤
        Microsoft::WRL::ComPtr<ID3D11SamplerState> SSAnisotropicWrap = nullptr;		        // 采样器状态：各项异性过滤

        Microsoft::WRL::ComPtr<ID3D11BlendState> BSNoColorWrite = nullptr;		                // 混合状态：不写入颜色
        Microsoft::WRL::ComPtr<ID3D11BlendState> BSTransparent = nullptr;		                // 混合状态：透明混合
        Microsoft::WRL::ComPtr<ID3D11BlendState> BSAlphaToCoverage = nullptr;	                // 混合状态：Alpha-To-Coverage
        Microsoft::WRL::ComPtr<ID3D11BlendState> BSAdditive = nullptr;			                // 混合状态：加法混合

        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSWriteStencil = nullptr;		        // 深度/模板状态：写入模板值
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSDrawWithStencil = nullptr;	        // 深度/模板状态：对指定模板值的区域进行绘制
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSNoDoubleBlend = nullptr;	        // 深度/模板状态：无二次混合区域
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSNoDepthTest = nullptr;		        // 深度/模板状态：关闭深度测试
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSNoDepthTestWriteDepth = nullptr;    // 深度/模板状态：关闭深度显示但是写入深度值
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSSNoDepthTestAndNoDepthWrite;// 深度/模板状态：仅深度测试，不写入深度值
    };
    static std::shared_ptr<MyStatus> status_ptr_;
};

};

#endif

#include "RenderStates.h"

namespace render {

std::shared_ptr<RenderStates::MyStatus> RenderStates::status_ptr_ = nullptr;

void RenderStates::destory() {
    status_ptr_.reset();
    status_ptr_ = nullptr;
}

void RenderStates::initAll(ID3D11Device* device) {
    // 先前初始化过的话就没必要重来了
    if (status_ptr_) {
        return;
    }
    status_ptr_ = std::make_shared<MyStatus>();
    // ******************
    // 初始化光栅化器状态
    //
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
    // 线框模式
    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthClipEnable = true;
    device->CreateRasterizerState(&rasterizerDesc, status_ptr_->RSWireframe.GetAddressOf());

    // 无背面剔除模式
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = false;
    device->CreateRasterizerState(&rasterizerDesc, status_ptr_->RSNoCull.GetAddressOf());
    {
        auto rst_desc = rasterizerDesc;
        rst_desc.DepthClipEnable = true;
        //if (DepthBiasClamp > 0)
        //    Bias = min(DepthBiasClamp, Bias)
        //else if (DepthBiasClamp < 0)
        //    Bias = max(DepthBiasClamp, Bias)
        //用于在深度测试之前将深度值偏移一个固定值，可以用来解决Z-fighting等深度冲突问题 正值会使物体远离相机，负值会使物体靠近相机
        //DepthBias 是一个常量偏移值，用来在深度比较之前或之后对深度值进行调整。该值可以正值、负值或零，通过增加或减少深度值来解决深度竞争问题。DepthBias 会对所有像素施加相同的偏移值，适用于处理简单的深度偏移情况
        rst_desc.DepthBias = -10;
        //用于限制深度偏移的最大值或最小值，避免过大或过小的偏移值导致不必要的效果。
        rst_desc.DepthBiasClamp = -0.05;
        //SlopeScaledDepthBias 的值指定了深度偏移量随着表面法线的变化而变化的速率。如果设置为 0，则表示不考虑斜率的影响，深度偏移的变化速率将与表面的法线无关。
        //当增加斜率相关深度偏移时，可以增加该值，使得对斜面的深度偏移更加敏感，从而避免了渲染时出现深度偏移引起的问题
        rst_desc.SlopeScaledDepthBias = 0;
        device->CreateRasterizerState(&rst_desc, status_ptr_->RSNoCullDepthOffset.GetAddressOf());
        rst_desc.DepthBias = -20;
        rst_desc.DepthBiasClamp = -0.1;
        device->CreateRasterizerState(&rst_desc, status_ptr_->RSNoCullDepthOffsetDouble.GetAddressOf());

    }
    {
        auto rst_desc = rasterizerDesc;
        rst_desc.DepthClipEnable = true;
        rst_desc.DepthBias = 10;
        rst_desc.DepthBiasClamp = 0.05;
        rst_desc.SlopeScaledDepthBias = 0;
        device->CreateRasterizerState(&rst_desc, status_ptr_->RSNoCullDepthOffsetAdd.GetAddressOf());

    }

    // 顺时针剔除模式
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = true;
    device->CreateRasterizerState(&rasterizerDesc, status_ptr_->RSCullClockWise.GetAddressOf());
    // 顺时针剔除模式
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_FRONT;
    rasterizerDesc.FrontCounterClockwise = true;
    device->CreateRasterizerState(&rasterizerDesc, status_ptr_->RSCullAntiClockwise.GetAddressOf());

    // ******************
    // 初始化采样器状态
    //
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));

    // 线性过滤模式
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, status_ptr_->SSLinearWrap.GetAddressOf());

    // 各向异性过滤模式
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
    // 初始化混合状态
    //
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    auto& rtDesc = blendDesc.RenderTarget[0];
    // Alpha-To-Coverage模式
    blendDesc.AlphaToCoverageEnable = true;
    blendDesc.IndependentBlendEnable = false;
    rtDesc.BlendEnable = false;
    rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    device->CreateBlendState(&blendDesc, status_ptr_->BSAlphaToCoverage.GetAddressOf());

    // 透明混合模式
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

    // 加法混合模式
    // RenderColor = SrcColor + DestColor
    // Alpha = SrcAlpha
    rtDesc.SrcBlend = D3D11_BLEND_ONE;
    rtDesc.DestBlend = D3D11_BLEND_ONE;
    rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

    device->CreateBlendState(&blendDesc, status_ptr_->BSAdditive.GetAddressOf());

    // 无颜色写入混合模式
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
    // 初始化深度/模板状态
    //
    D3D11_DEPTH_STENCIL_DESC dsDesc;

    // 写入模板值的深度/模板状态
    // 这里不写入深度信息
    // 无论是正面还是背面，原来指定的区域的模板值都会被写入StencilRef
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
    // 对于背面的几何体我们是不进行渲染的，所以这里的设置无关紧要
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSWriteStencil.GetAddressOf());

    // 对指定模板值进行绘制的深度/模板状态
    // 对满足模板值条件的区域才进行绘制，并更新深度
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    // 对于背面的几何体我们是不进行渲染的，所以这里的设置无关紧要
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSDrawWithStencil.GetAddressOf());

    // 无二次混合深度/模板状态
    // 允许默认深度测试
    // 通过自递增使得原来StencilRef的值只能使用一次，实现仅一次混合
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
    // 对于背面的几何体我们是不进行渲染的，所以这里的设置无关紧要
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSNoDoubleBlend.GetAddressOf());

    // 关闭深度测试的深度/模板状态
    // 若绘制非透明物体，务必严格按照绘制顺序
    // 绘制透明物体则不需要担心绘制顺序
    // 而默认情况下模板测试就是关闭的
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

    // 进行深度测试，但不写入深度值的状态
    // 若绘制非透明物体时，应使用默认状态
    // 绘制透明物体时，使用该状态可以有效确保混合状态的进行
    // 并且确保较前的非透明物体可以阻挡较后的一切物体
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.StencilEnable = false;

    (device->CreateDepthStencilState(&dsDesc, status_ptr_->DSSNoDepthTestAndNoDepthWrite.GetAddressOf()));
    // ******************
    // 设置调试对象名
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
}  // namespace render

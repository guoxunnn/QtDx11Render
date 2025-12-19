#ifndef DX11COMMONFUNC_H
#define DX11COMMONFUNC_H

#include <DirectXMath.h>
#include <assert.h>
#include <guiddef.h>
#include <stdint.h>
#include <wrl.h>

#include <iostream>

#include "d3d11.h"
#include "d3dcompiler.h"

class ID3D11DeviceContext;
class ID3D11Resource;

class DX11CommonFunc {
public:
    // static long SaveDDSTextureToFile(ID3D11DeviceContext* pContext, ID3D11Resource* pSource,
    //                                  const wchar_t* fileName);
    static void DX11ReadPixels(ID3D11DeviceContext* pContext, ID3D11Resource* pSource, int pX, int pY,
                               uint8_t pColor[4]);
    /**
     * @brief DX11ReadImage
     * @param pContext
     * @param pSource
     * @param pX 图像起点
     * @param pY 图像的重点
     * @param width 图像宽度
     * @param height 图像高度
     * @param pColor 所有数据存储在这个里面，内存由用户开辟
     */
    static void DX11ReadImage(ID3D11DeviceContext* pContext, ID3D11Resource* pSource, uint32_t pX,
                              uint32_t pY, uint32_t width, uint32_t height, unsigned char* pColor);
};

#endif  // DX11COMMONFUNC_H

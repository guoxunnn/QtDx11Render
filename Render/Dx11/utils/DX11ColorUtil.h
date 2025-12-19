#ifndef DX11ColorUtil_H
#define DX11ColorUtil_H

#include <DirectXMath.h>

namespace render {

class DX11ColorUtil {
public:
    static DirectX::XMFLOAT4 GetXMFLOAT4(RenderColor color) {
        return DirectX::XMFLOAT4(color.Red(), color.Green(), color.Blue(), color.Alpha());
    }
};

}  // namespace render

#endif

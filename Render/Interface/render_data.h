#pragma once


class ID3D11Device;
class ID3D11DeviceContext;

class RenderInterface {
public:
    #if defined(_WIN32) || defined(_WIN32_) || defined(WIN32) || defined(_WIN64_) || defined(WIN64) || \
    defined(_WIN64)
    ID3D11Device* drv = nullptr;
    ID3D11DeviceContext* drv_context = nullptr;
    #elif defined(__APPLE__)
    void* cb = nullptr;
    void* cmd_queue = nullptr;
    void* edt10 = nullptr;
    // MTLDevice代表GPU设备，提供创建缓存、纹理等的接口，在初始化时候需要赋给MTKView
    void* drv = nullptr;
    #endif
};

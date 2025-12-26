#pragma once
#include <stdint.h>

class ID3D11Device;
class ID3D11DeviceContext;
namespace render {
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

    struct Point {
        int x = 0, y = 0;
        Point() {
        }
        Point(int x, int y) : x(x), y(y) {
        }
    };

    struct MouseEvent {
        enum Button {
            NoButton = 0x00000000,
            LeftButton = 0x00000001,
            RightButton = 0x00000002,
            MiddleButton = 0x00000004,
            BackButton = 0x00000008
        };
        enum Modifier {
            NoModifier = 0x00000000,
            ShiftModifier = 0x02000000,
            ControlModifier = 0x04000000,
            AltModifier = 0x08000000,
            MetaModifier = 0x10000000,
            KeypadModifier = 0x20000000
        };
        bool isCtrlPressed() const {
            return modifiers_ & ControlModifier;
        }
        bool isShiftPressed() const {
            return modifiers_ & ShiftModifier;
        }
        bool isAltPressed() const {
            return modifiers_ & AltModifier;
        }
        Button buttons;
        uint32_t modifiers_;
        Point position;
        int win_id_ = -1;
    };
}

#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "framework/ximage.h"
#include "framework/xvector.h"
#include "geometry/FVector3D.h"
#include "geometry/FVector2D.h"

// 支撑和模型面片相距
#define SupDistanceModelNum 3

namespace render {
    class RenderColor {
    public:
        RenderColor();
        RenderColor(float r, float g, float b, float a = 1.f);

        static RenderColor FromColor32(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        static RenderColor FromColor32(uint32_t rgba);

        float Red() const { return red_; }
        float Green() const { return green_; }
        float Blue() const { return blue_; }
        float Alpha() const { return alpha_; }
        const float* Rgba() const { return rgba_; }

        void SetRed(float r) { red_ = r; }
        void SetGreen(float g) { green_ = g; }
        void SetBlue(float b) { blue_ = b; }
        void SetAlpha(float a) { alpha_ = a; }

        void SetRgb(const float rgb[3]) { SetRgb(rgb[0], rgb[1], rgb[2]); }
        void SetRgb(float r, float g, float b) {
            red_ = r;
            green_ = g;
            blue_ = b;
        }

        void SetRgba(const float rgba[4]) { SetRgba(rgba[0], rgba[1], rgba[2], rgba[3]); }
        void SetRgba(float r, float g, float b, float a) {
            red_ = r;
            green_ = g;
            blue_ = b;
            alpha_ = a;
        }

        bool operator==(const RenderColor& other) const;
        bool operator!=(const RenderColor& other) const;

    private:
        union {
            float rgba_[4];
            struct { float red_, green_, blue_, alpha_; };
        };
    };

    class RenderMouseAndKeyEventControl;
    // 渲染数据基类
    class RenderBufferBase {
    public:
        virtual ~RenderBufferBase() {
        }
    };

    class RenderTextureBase {
    public:
        virtual ~RenderTextureBase() {
        }
        unsigned int width = 0;
        unsigned int height = 0;
        int mip_level_ = 1;
    };

    class RenderUavBase {
    public:
        virtual ~RenderUavBase() = default;

    public:
        inline int GetCount() const {
            return count_;
        }
        inline int GetSingleElementSize() const {
            return single_element_size_;
        }
        inline int GetSlot() const {
            return slot_;
        }

    protected:
        // 元素个数
        int count_ = 0;
        // 单元素大小
        int single_element_size_ = 0;
        // 绑定槽
        int slot_ = 0;
    };

    struct BlockBuffer {
        BlockBuffer(int64_t k, const uint32_t* i) : indices(i), model_id(k) {
        }

    public:
        std::shared_ptr<RenderBufferBase> vertex_handle_ = nullptr;  // 顶点缓存句柄
        std::shared_ptr<RenderBufferBase> index_handle_ = nullptr;   // 索引缓存句柄
        // 三角形状态的顶点索引句柄
        std::shared_ptr<RenderBufferBase> tri_status_index_handle_ = nullptr;  // 索引缓存句柄
        bool is_exist_flag_ = true;                                            // 内部使用
        int64_t start_index_ = 0;
        uint32_t vertex_size = 0;
        uint32_t index_size = 0;
        // 选择面片会用到下面两个
        uint32_t normal_index_size = 0;
        uint32_t select_index_size = 0;
        uint32_t hide_index_size = 0;
        const float* vertexes;
        const uint32_t* indices;
        //  const uint32_t *mapKey;
        int64_t model_id;
        unsigned int stride_ = sizeof(float) * 3;
    };

    struct VertexBlockBuffer {
        // 顶点缓存句柄
        std::shared_ptr<RenderBufferBase> vertex_handle_ = nullptr;
        uint32_t vertex_size = 0;
    };

    struct VertexPosNorlTex {
        geometry::FVector3D pos;  // 顶点坐标
        geometry::FVector3D normal;
        geometry::FVector2D tex;  // 纹理坐标
    };
}// namespace render

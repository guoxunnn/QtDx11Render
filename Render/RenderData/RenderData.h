#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../Interface/render_data.h"
#include "Render/Threadpart/framework/ximage.h"
#include "Render/Threadpart/framework/xvector.h"

// 支撑和模型面片相距
#define SupDistanceModelNum 3
namespace base {
class ThreadPool;
}
namespace render {
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
}// namespace render

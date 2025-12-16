#pragma once
#include "RenderData.h"
#include "../../utils/GeometryHelper.h"

namespace render {
class RenderContext;
class RenderEngineInterface;

class ComRenderResource {
public:
    void RefreshData(RenderContext* render_context);
    static std::shared_ptr<BlockBuffer> createBuffer(RenderEngineInterface* engine_interface,
        std::pair<std::vector<VertexPosNorlTex>, std::vector<int>> d);
    static std::shared_ptr<BlockBuffer> createBuffer(RenderEngineInterface* engine_interface, const std::vector<FVector3D>& vertices, const std::vector<int>& indices);
private:
    void refreshRTVData(RenderContext* render_context);
    void refreshPlatformGridTexture(RenderContext* render_context);
    void refreshPlatformGridData(RenderContext* render_context);
    using VertexPosNormalTexVecPiar = std::pair<std::vector<VertexPosNorlTex>, std::vector<int>>;
    VertexPosNormalTexVecPiar createPlatformDir(const PlatFormDataConfig& platform_data, float w, float h, float z);

public:
    VertexPosNormalTexVecPiar createPlane(float w, float h, float z, float top,
                                                                           float bottom, float left, float right,
                                                                           float texCoordX, float texCoordY);
public:
    std::shared_ptr<BlockBuffer> rtv_data_ = nullptr;
    std::shared_ptr<BlockBuffer> platform_grid_data_ = nullptr;
    std::shared_ptr<BlockBuffer> platform_dir_data_ = nullptr;
    std::shared_ptr<RenderTextureBase> platform_grid_left_top_texture = nullptr;
    std::shared_ptr<RenderTextureBase> platform_grid_left_top_small_texture = nullptr;
    std::shared_ptr<RenderTextureBase> platform_grid_out_rect_texture_ = nullptr;
    std::shared_ptr<RenderTextureBase> platform_dir_texture_ = nullptr;

    int64_t platform_grid_updata_tick_ = -1;
    int64_t texture_updata_tick_ = -1;
    RenderConfig::LanguageType current_language_type_;
    bool is_init_ = false;
};

}  // namespace render

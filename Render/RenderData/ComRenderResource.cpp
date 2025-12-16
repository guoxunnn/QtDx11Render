#include "ComRenderResource.h"
#include "../../manager/RenderEngineInterface.h"
#include "../../base/RenderContext.h"
#include "../../manager/SixViewManager.h"
#include "../../manager/WindowOperatorManager.h"

#include "../../utils/ConfigHelper.h"
#include "../ModelRenderManager.h"

namespace render {
ComRenderResource::VertexPosNormalTexVecPiar ComRenderResource::createPlane(float w, float h, float z, float top,
                                                                            float bottom, float left, float right,
                                                                            float texCoordX, float texCoordY) {
    left = -(w / 2 - left);
    right = w / 2 - right;
    if (bottom > 0 || top > 0) {
        top = (h / 2 - top);
        bottom = -h / 2 + bottom;
    }
    else {
        //为了适配rtv
        top = -(h / 2 - top);
        bottom = h / 2 - bottom;
    }
    std::vector<VertexPosNorlTex> vertexDataArr;
    vertexDataArr.resize(4);
    vertexDataArr[0].pos = FVector3D(left, top, z);
    vertexDataArr[0].normal = FVector3D(0, 0, 1);
    vertexDataArr[0].tex = FVector2D(0, texCoordY);

    vertexDataArr[1].pos = FVector3D(left, bottom, z);
    vertexDataArr[1].normal = FVector3D(0, 0, 1);
    vertexDataArr[1].tex = FVector2D(0, 0);

    vertexDataArr[2].pos = FVector3D(right, bottom, z);
    vertexDataArr[2].normal = FVector3D(0, 0, 1);
    vertexDataArr[2].tex = FVector2D(texCoordX, 0);

    vertexDataArr[3].pos = FVector3D(right, top, z);
    vertexDataArr[3].normal = FVector3D(0, 0, 1);
    vertexDataArr[3].tex = FVector2D(texCoordX, texCoordY);
    std::vector<int> vecIndex = {0, 1, 2, 2, 3, 0};
    std::pair<std::vector<VertexPosNorlTex>, std::vector<int>> pair;
    pair.first = static_cast<std::vector<VertexPosNorlTex>&&>(vertexDataArr);
    pair.second = static_cast<std::vector<int>&&>(vecIndex);
    return pair;
}

ComRenderResource::VertexPosNormalTexVecPiar ComRenderResource::createPlatformDir(
    const PlatFormDataConfig& platform_data, float w, float h, float z) {
    std::vector<VertexPosNorlTex> vertexDataArr;
    vertexDataArr.resize(4);
    /**
     * p1***********p2
     * *            *
     * *            *
     * *            *
     * p3***********p4
     */
    FVector3D p1;
    FVector3D p2;
    FVector3D p3;
    FVector3D p4;

    PlatFormDataConfig::PlatformDirection dir = platform_data.platformDirection;
    float machineWidth = platform_data.machineWidth;
    float machineDepth = platform_data.machineDepth;
    float top = platform_data.buildAreaOffsetTop;
    float bottom = platform_data.buildAreaOffsetBottom;
    float left = platform_data.buildAreaOffsetLeft;
    float right = platform_data.buildAreaOffsetRight;
    if (!platform_data.buildAreaOffsetEnable) {
        top = 0;
        bottom = 0;
        left = 0;
        right = 0;
    }
    float offset_x = (left - right) / 2;
    float offset_y = (bottom - top) / 2;
    machineDepth = machineDepth - (bottom + top);
    machineWidth = machineWidth - (left + right);
    // ��һ��
    if (dir == PlatFormDataConfig::Bottom) {
        p1 = FVector3D(-w / 2, (machineDepth / 2 + h), z);
        p2 = FVector3D(w / 2, (machineDepth / 2 + h), z);
        p3 = FVector3D(-w / 2, (machineDepth / 2), z);
        p4 = FVector3D(w / 2, (machineDepth / 2), z);
    } else if (dir == PlatFormDataConfig::Left) {
        p1 = FVector3D(-(machineWidth / 2) - w, h / 2, z);
        p2 = FVector3D(-(machineWidth / 2), h / 2, z);
        p3 = FVector3D(-(machineWidth / 2) - w, -h / 2, z);
        p4 = FVector3D(-(machineWidth / 2), -h / 2, z);
    } else if (dir == PlatFormDataConfig::Right) {
        p1 = FVector3D(machineWidth / 2, h / 2, z);
        p2 = FVector3D(machineWidth / 2 + w, h / 2, z);
        p3 = FVector3D(machineWidth / 2, -h / 2, z);
        p4 = FVector3D(machineWidth / 2 + w, -h / 2, z);
    } else if (dir == PlatFormDataConfig::Top) {
        p1 = FVector3D(-w / 2, -(machineDepth / 2), z);
        p2 = FVector3D(w / 2, -(machineDepth / 2), z);
        p3 = FVector3D(-w / 2, -(machineDepth / 2) - h, z);
        p4 = FVector3D(w / 2, -(machineDepth / 2 ) - h, z);
    }

    p1 += geometry::FVector3D(offset_x, offset_y, 0);
    p2 += geometry::FVector3D(offset_x, offset_y, 0);
    p3 += geometry::FVector3D(offset_x, offset_y, 0);
    p4 += geometry::FVector3D(offset_x, offset_y, 0);

    // p3
    vertexDataArr[0].pos = p3;
    vertexDataArr[0].normal = FVector3D(0, 0, 1);
    vertexDataArr[0].tex = FVector2D(0, 1);
    // p1
    vertexDataArr[1].pos = p1;
    vertexDataArr[1].normal = FVector3D(0, 0, 1);
    vertexDataArr[1].tex = FVector2D(0, 0);
    // p2
    vertexDataArr[2].pos = p2;
    vertexDataArr[2].normal = FVector3D(0, 0, 1);
    vertexDataArr[2].tex = FVector2D(1, 0);
    // p4
    vertexDataArr[3].pos = p4;
    vertexDataArr[3].normal = FVector3D(0, 0, 1);
    vertexDataArr[3].tex = FVector2D(1, 1);
    std::vector<int> vecIndex = {0, 1, 2, 2, 3, 0};
    std::pair<std::vector<VertexPosNorlTex>, std::vector<int>> pair;
    pair.first = static_cast<std::vector<VertexPosNorlTex>&&>(vertexDataArr);
    pair.second = static_cast<std::vector<int>&&>(vecIndex);
    return pair;
}

void ComRenderResource::RefreshData(RenderContext* render_context) {
    refreshRTVData(render_context);
    refreshPlatformGridData(render_context);
    refreshPlatformGridTexture(render_context);
}

std::shared_ptr<BlockBuffer> ComRenderResource::createBuffer(
    RenderEngineInterface* engine_interface, std::pair<std::vector<VertexPosNorlTex>, std::vector<int>> d) {
    std::vector<VertexPosNorlTex> v_dat = d.first;
    std::vector<int> v_idx = d.second;
    auto buffer = std::make_shared<BlockBuffer>(-1, nullptr);
    int b_size = v_dat.size() * sizeof(VertexPosNorlTex) / sizeof(float);
    engine_interface->CreateGpuVertexBuffer((float*)v_dat.data(), b_size, buffer->vertex_handle_);
    engine_interface->CreateGpuIndexBuffer((uint32_t*)v_idx.data(), v_idx.size(),
                                                            buffer->index_handle_);
    buffer->index_size = v_idx.size();
    buffer->stride_ = sizeof(VertexPosNorlTex);
    return buffer;
}

std::shared_ptr<BlockBuffer> ComRenderResource::createBuffer(
    RenderEngineInterface* engine_interface, const std::vector<FVector3D>& vertices, const std::vector<int>& indices) {
    auto buffer = std::make_shared<BlockBuffer>(-1, nullptr);

    int vertexBufferSize = vertices.size() * sizeof(FVector3D) / sizeof(float);
    engine_interface->CreateGpuVertexBuffer((float*)vertices.data(), vertexBufferSize, buffer->vertex_handle_);
    engine_interface->CreateGpuIndexBuffer((uint32_t*)indices.data(), indices.size(), buffer->index_handle_);

    buffer->index_size = indices.size();
    buffer->stride_ = sizeof(FVector3D);

    return buffer;
}

void ComRenderResource::refreshPlatformGridTexture(RenderContext* render_context) {
    const PlatFormDataConfig& platform_data = render_context->render_config_->platform_data_config;
    const auto renderInterface = render_context->GetRenderEngineInterface();
    if (render_context->render_config_->GetPlatformTextureUpdateTick() == texture_updata_tick_
        && platform_data.GetPlatformRenderUpdataTick() == platform_grid_updata_tick_) {
        return;
    }

    platform_grid_updata_tick_ = platform_data.GetPlatformRenderUpdataTick();
    texture_updata_tick_ = render_context->render_config_->GetPlatformTextureUpdateTick();
    float width = platform_data.machineWidth;
    float height = platform_data.machineDepth;
    float ration_wh = width / height;
    const auto& platform_config = render_context->GetRenderColorConfig().platform_render_config_;

    {
        int lineWidth = 1;
        const auto& bg = platform_config.platform_background_color_;
        const auto& lc = platform_config.platform_line_color_;
        framework::XVector<Image> image_vec =
            render_context->create_rect_texture_(1024, 1024, algo::AlgoColor32::FromColorF(lc).Rgba(), lineWidth,
                                                 algo::AlgoColor32::FromColorF(bg).Rgba(), true);
        this->platform_grid_left_top_texture = renderInterface->CreateTexutre(image_vec);
        auto image_vec2 =
            render_context->create_rect_texture_(64, 64, algo::AlgoColor32::FromColorF(lc).Rgba(), lineWidth,
                                                 algo::AlgoColor32::FromColorF(bg).Rgba(), false);
        this->platform_grid_left_top_small_texture = renderInterface->CreateTexutre(image_vec2);
    }

    {
        const auto& bg = platform_config.platform_background_color_;
        const auto& lc = platform_config.platform_out_rect_color_;
        float lineWidth = 0.01;
        auto image_vec_out_rect =
            render_context->create_out_rect_(1000 * ration_wh, 1000 , lineWidth, algo::AlgoColor32::FromColorF(lc).Rgba(),
                                             algo::AlgoColor32::FromColorF(bg).Rgba());
        platform_grid_out_rect_texture_ = renderInterface->CreateTexutre(image_vec_out_rect);
    }

    {
        const auto& bg = platform_config.platform_background_color_;
        const auto& lc = platform_config.platform_out_rect_color_;
        const auto& tc = platform_config.platform_out_rect_text_color_;
        auto rc = bg;
        float topRatio = 0.6;
        float heightRatio = 0.2;
        float fontRatio = 0.1;
        rc.SetAlpha(0);
        const PlatFormDataConfig& platform_data = render_context->render_config_->platform_data_config;
        framework::XVector<Image> dir_vec = render_context->create_platform_dir_texture_(
            1000, 1000, algo::AlgoColor32::FromColorF(lc).Rgba(), algo::AlgoColor32::FromColorF(rc).Rgba(),
            algo::AlgoColor32::FromColorF(tc).Rgba(), topRatio, heightRatio, fontRatio,
            platform_data.platformDirection);
        platform_dir_texture_ = renderInterface->CreateTexutre(dir_vec);
    }
}

void ComRenderResource::refreshRTVData(RenderContext* render_context) {
    if (!rtv_data_) {
        rtv_data_ = ComRenderResource::createBuffer(render_context->GetRenderEngineInterface().get(), createPlane(2, 2, 0, 0, 0, 0, 0, 1, 1));
    }
}

void ComRenderResource::refreshPlatformGridData(RenderContext* render_context) {
    const PlatFormDataConfig& platform_data = render_context->render_config_->platform_data_config;
    if (platform_data.GetPlatformRenderUpdataTick() == platform_grid_updata_tick_) {
        return;
    }
    platform_grid_updata_tick_ = platform_data.GetPlatformRenderUpdataTick();
    {
        float width = platform_data.machineWidth;
        float height = platform_data.machineDepth;
        float z = platform_data.platformZOffset;
        float top = platform_data.buildAreaOffsetTop;
        float bottom = platform_data.buildAreaOffsetBottom;
        float left = platform_data.buildAreaOffsetLeft;
        float right = platform_data.buildAreaOffsetRight;
        if (!platform_data.buildAreaOffsetEnable) {
            top = 0;
            bottom = 0;
            left = 0;
            right = 0;
        }
        float hGridNum = platform_data.hGridNum;
        float vGridNum = platform_data.vGridNum;

        auto platform_grid_data = createPlane(width, height, z, top, bottom, left, right, hGridNum, vGridNum);
        platform_grid_data_ = ComRenderResource::createBuffer(render_context->GetRenderEngineInterface().get(), platform_grid_data);
    }
    // ����
    {
        float ration = 0.1;
        float ori_w =
            (platform_data.machineWidth - platform_data.buildAreaOffsetLeft - platform_data.buildAreaOffsetRight);
        float w = ori_w * ration;
        // ���ori_w��һ�����50����һֱ����50�������ʼС��50����һֱ��һ��
        if (w < 50) {
            if (50 < ori_w * 0.5) {
                w = 50;
            } else {
                w = ori_w * 0.5;
            }
        }
        float zDepth = platform_data.platformZOffset;
        auto platform_dir_data = createPlatformDir(platform_data, w, w, zDepth);
        platform_dir_data_ = ComRenderResource::createBuffer(render_context->GetRenderEngineInterface().get(), platform_dir_data);
    }
}

}  // namespace render

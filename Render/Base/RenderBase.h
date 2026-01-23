
#ifndef RENDERBASE_H
#define RENDERBASE_H
#include "geometry/FMatrix4x4.h"
#include <geometry/DMatrix4x4.h>
#include "framework/ximage.h"
#include <string>
#include <memory>
#include "Core/Buffer/ModelBuffer.h"
#include "../RenderData/RenderData.h"
#include "Base/RenderEngineInterface.h"
#include "../RenderData/RenderContext.h"

namespace  render {
class RenderAreaBase;
class ShaderRenderMat4x42 {
public:
    ShaderRenderMat4x42() {
    }
    ShaderRenderMat4x42(const geometry::FMatrix4x4& mat) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                data[y][x] = *(mat.constData() + x + (y * 4));
            }
        }
    }
    ShaderRenderMat4x42(const geometry::DMatrix4x4& mat) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                data[y][x] = *(mat.constData() + x + (y * 4));
            }
        }
    }
    ShaderRenderMat4x42(const geometry::DMatrix3x3& mat) {
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                data[y][x] = *(mat.constData() + x + (y * 3));
            }
        }
    }

private:
    float data[4][4] = {0};
};

//metal和opengl需要4float对其
struct ShaderPickInfo {
    geometry::FVector4D vert;
    geometry::FVector4D norml;
    int pick_flag = 0;
    int fill[3] = { 0 };
};
class RenderBase {
public:
    struct MatCB {
        // 4x4矩阵
        ShaderRenderMat4x42 finallyMatrix;
        ShaderRenderMat4x42 worldMatrix;
        ShaderRenderMat4x42 worldNormalMatrix;
        ShaderRenderMat4x42 modelMatrix;
        ShaderRenderMat4x42 modelNormalMatrix;
        ShaderRenderMat4x42 projMatrix;
        int                 uav_pick_flag_ = 0;
        int                 uav_pos_x = -1;
        int                 uav_pos_y = -1;
        int                 win_height = 0;
    };
    RenderBase(RenderAreaBase* engine);
    // 不允许拷贝，允许移动
    RenderBase(const RenderBase&) = delete;
    RenderBase& operator=(const RenderBase&) = delete;
    RenderBase(RenderBase&&) = default;
    RenderBase& operator=(RenderBase&&) = default;
    virtual void Render() = 0;
    //释放纹理，防止内存占用
    void RelaseOffTexture();

protected:
    virtual void updataConstVar();
    virtual bool init();
    virtual void release();
    void renderInit();
    std::shared_ptr<RenderEngineInterface> getRenderEngineInterface() const;
    std::shared_ptr<RenderUavBase> CreateShaderPickUav(int slot);
    void    BindShaderPickUav(std::shared_ptr<RenderUavBase> ptr);
    ShaderPickInfo GetPickInfo(std::shared_ptr<RenderUavBase>);
    void beginMultTextureRender(const RenderColor& clear_color, int width, int height, bool is_clear_dep = false);
    void endMultTextureRender();
    virtual void renderModelBuffer(ModelBuffer* model_buf, RenderEngineInterface::DrawType type = RenderEngineInterface::DrawTriangle);
    virtual void renderBlockBuffer(BlockBuffer* buf, RenderEngineInterface::DrawType type);
    virtual void renderBlockBuffer(VertexBlockBuffer* buf, RenderEngineInterface::DrawType type = RenderEngineInterface::DrawLine);

    bool beginOffScreenRender(const RenderColor& clear_color, int width = 0, int height = 0);
    void endOffScreenRender();
    //销毁长时间不使用的纹理 节省空间 销毁成功返回true
    bool destoryLongTimeUnuseTexture();
    void fmat4x4Printf(const std::string& name, const geometry::DMatrix4x4& mat);

public:
    RenderConstObject<MatCB> mat_cb_;
    RenderAreaBase* render_engine_ = nullptr;
    RenderContext* render_context_ = nullptr;
    bool is_init_flag_ = false;
    bool is_init_belong_render_flag_ = false;
    RenderStatusInfo status_info_;
    std::shared_ptr<RenderTargetTextureBase> offscreen_texture_ = nullptr;
    std::shared_ptr<RenderProgramBase> shader_program_ = nullptr;
    RenderEngineInterface::Viewport last_view_port_;
    long long create_render_texture_time_ = 0;

};
}

#endif // RENDERBASE_H


#ifndef MODELRENDERMANAGER_H
#define MODELRENDERMANAGER_H
#include <unordered_map>
#include <memory>
#include "../../RenderData/RenderData.h"
#include "geometry/DMatrix4x4.h"
#include "../../RenderData/RenderModel.h"
#include "../Buffer/ModelBuffer.h"

namespace render {
class RenderContext;



class ModelRenderManager {
public:
    ModelRenderManager();
    void BuildModelGpuBuffer(std::shared_ptr<RenderContext> context);
    std::shared_ptr<ModelBuffer> CreateModelBufferFromRenderModelData(RenderModel* m, std::shared_ptr<RenderContext> context);
    std::shared_ptr<RenderModel> LoadStlFile(const char* filePath, bool is_need_place_on_platform_flag);

private:
    std::vector<std::shared_ptr<RenderModel>> test_render_model_vec;
    std::unordered_map<int64_t, std::shared_ptr<ModelBuffer>> test_model_bufer_map_;
};
}

#endif // MODELRENDERMANAGER_H


#ifndef MODELBUFFER_H
#define MODELBUFFER_H

#include <unordered_map>
#include <memory>
#include "../../RenderData/RenderData.h"
#include "geometry/DMatrix4x4.h"

namespace render {
class ModelBuffer {
public:
    int64_t model_id_ = -1;
    geometry::DMatrix4x4 model_matrix_;
    std::vector<std::shared_ptr<BlockBuffer>> block_buffers_;
};
}


#endif // MODELBUFFER_H

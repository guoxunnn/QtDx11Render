#include "RenderContext.h"
namespace render {
RenderContext::RenderContext(RenderApiImpl* impl, std::shared_ptr<RenderEngineInterface> engine):impl_(impl), engine_(engine) {

}
}


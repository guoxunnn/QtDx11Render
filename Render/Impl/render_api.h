#ifndef RENDER_API_H
#define RENDER_API_H
#include "render_config.h"

namespace render {
class RenderApi {
public:
    RenderApi();
    virtual void Render(int win_id) = 0;
    virtual RenderConfig* GetRenderConfig() const = 0;
};
}


#endif // RENDER_API_H

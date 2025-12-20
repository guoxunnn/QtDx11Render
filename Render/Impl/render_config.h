#ifndef RENDER_CONFIG_H
#define RENDER_CONFIG_H
#include "render_color_config.h"

namespace render {
class RenderConfig {
public:
    RenderConfig();

public:
    RenderColorConfig render_color_config_;
};
}


#endif // RENDER_CONFIG_H

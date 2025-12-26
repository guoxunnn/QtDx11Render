
#ifndef MAINAREARENDER_H
#define MAINAREARENDER_H
#include "RenderAreaBase.h"

namespace render {
class MainAreaRender : public RenderAreaBase {
public:
    MainAreaRender(RenderManager* manager): RenderAreaBase(manager){}
    void Render() override;
};
}


#endif // MAINAREARENDER_H

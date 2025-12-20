#ifndef RENDER_API_H
#define RENDER_API_H
namespace render {
class RenderApi {
public:
    RenderApi();
    virtual void Render() = 0;
};
}


#endif // RENDER_API_H

#ifndef RENDERWINDOWVIEW_H
#define RENDERWINDOWVIEW_H

class RenderItemBase;

class RenderWindowView {
public:
    RenderWindowView(int win_id);
    void AddRenderItem(RenderItemBase* item);
};

#endif // RENDERWINDOWVIEW_H

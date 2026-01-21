include(DX11/DX11.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/Base/MainAreaRender.h \
    $$PWD/Base/RenderAreaBase.h \
    $$PWD/Base/RenderBase.h \
    $$PWD/Base/RenderEngineInterface.h \
    $$PWD/Core/Buffer/ModelBuffer.h \
    $$PWD/Core/Manager/ModelRenderManager.h \
    $$PWD/Core/Manager/RenderManager.h \
    $$PWD/Core/Manager/RenderMouseAndKeyEventControl.h \
    $$PWD/Core/render_api_impl.h \
    $$PWD/Impl/render_api.h \
    $$PWD/Impl/render_color_config.h \
    $$PWD/Impl/render_config.h \
    $$PWD/Impl/render_data.h \
    $$PWD/RenderData/RCamera.h \
    $$PWD/RenderData/RenderContext.h \
    $$PWD/RenderData/RenderData.h \
    $$PWD/RenderData/RenderModel.h

SOURCES += \
    $$PWD/Base/MainAreaRender.cpp \
    $$PWD/Base/RenderAreaBase.cpp \
    $$PWD/Base/RenderBase.cpp \
    $$PWD/Base/RenderEngineInterface.cpp \
    $$PWD/Core/Buffer/ModelBuffer.cpp \
    $$PWD/Core/Manager/ModelRenderManager.cpp \
    $$PWD/Core/Manager/RenderManager.cpp \
    $$PWD/Core/Manager/RenderMouseAndKeyEventControl.cpp \
    $$PWD/Core/render_api_impl.cpp \
    $$PWD/Impl/render_api.cpp \
    $$PWD/Impl/render_color_config.cpp \
    $$PWD/Impl/render_config.cpp \
    $$PWD/RenderData/RCamera.cpp \
    $$PWD/RenderData/RenderContext.cpp \
    $$PWD/RenderData/RenderData.cpp \
    $$PWD/RenderData/RenderModel.cpp


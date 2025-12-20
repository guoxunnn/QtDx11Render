include(DX11/DX11.pri)

HEADERS += \
    $$PWD/Base/RenderEngineInterface.h \
    $$PWD/Core/render_api_impl.h \
    $$PWD/Impl/render_api.h \
    $$PWD/Impl/render_color_config.h \
    $$PWD/Impl/render_config.h \
    $$PWD/Impl/render_data.h

SOURCES += \
    $$PWD/Base/RenderEngineInterface.cpp \
    $$PWD/Core/render_api_impl.cpp \
    $$PWD/Impl/render_api.cpp \
    $$PWD/Impl/render_color_config.cpp \
    $$PWD/Impl/render_config.cpp


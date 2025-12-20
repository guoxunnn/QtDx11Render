LIBS += -ld3d11 -ld3dcompiler

HEADERS += \
    $$PWD/DX11RenderEngineInterface.h \
    $$PWD/texture/Texture2D.h \
    $$PWD/utils/D3DUtil.h \
    $$PWD/utils/DX11ColorUtil.h \
    $$PWD/utils/DX11CommonFunc.h \
    $$PWD/utils/DX11Vertex.h \
    $$PWD/utils/RenderStates.h

SOURCES += \
    $$PWD/DX11RenderEngineInterface.cpp \
    $$PWD/texture/Texture2D.cpp \
    $$PWD/utils/D3DUtil.cpp \
    $$PWD/utils/DX11CommonFunc.cpp \
    $$PWD/utils/DX11Vertex.cpp \
    $$PWD/utils/RenderStates.cpp

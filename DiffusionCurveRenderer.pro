QT += core gui xml openglwidgets opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += USE_QPAINTER

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Controller.h \
    CurveContainer.h \
    Curves/Bezier.h \
    Curves/ColorPoint.h \
    Curves/ControlPoint.h \
    Curves/Curve.h \
    CustomVariant.h \
    Renderers/Base/Quads.h \
    Renderers/Base/Ticks.h \
    Renderers/ContourRenderer.h \
    Renderers/Diffusion/ColorRenderer.h \
    Renderers/Diffusion/DownsampleRenderer.h \
    Renderers/Diffusion/JacobiRenderer.h \
    Renderers/Diffusion/UpsampleRenderer.h \
    Renderers/GUI/Base/LineRenderer.h \
    Renderers/GUI/Base/PointRenderer.h \
    Renderers/GUI/Base/RectangleRenderer.h \
    Renderers/GUI/BoundingBoxRenderer.h \
    Renderers/GUI/ControlPointRenderer.h \
    Renderers/RendererManager.h \
    Renderers/ScreenRenderer.h \
    Transformer.h \
    Types.h \
    Util.h \
    Widgets/CentralWidget.h \
    Widgets/ColorPointWidget.h \
    Widgets/ControlPointWidget.h \
    Widgets/CurveWidget.h \
    Widgets/CustomFlatButton.h \
    Widgets/ModeWidget.h \
    Widgets/OpenGLWidget.h \
    Widgets/RenderSettingsWidget.h \
    Widgets/ZoomWidget.h \
    Window.h

SOURCES += \
    Controller.cpp \
    CurveContainer.cpp \
    Curves/Bezier.cpp \
    Curves/ColorPoint.cpp \
    Curves/ControlPoint.cpp \
    Curves/Curve.cpp \
    CustomVariant.cpp \
    Main.cpp \
    Renderers/Base/Quads.cpp \
    Renderers/Base/Ticks.cpp \
    Renderers/ContourRenderer.cpp \
    Renderers/Diffusion/ColorRenderer.cpp \
    Renderers/Diffusion/DownsampleRenderer.cpp \
    Renderers/Diffusion/JacobiRenderer.cpp \
    Renderers/Diffusion/UpsampleRenderer.cpp \
    Renderers/GUI/Base/LineRenderer.cpp \
    Renderers/GUI/Base/PointRenderer.cpp \
    Renderers/GUI/Base/RectangleRenderer.cpp \
    Renderers/GUI/BoundingBoxRenderer.cpp \
    Renderers/GUI/ControlPointRenderer.cpp \
    Renderers/RendererManager.cpp \
    Renderers/ScreenRenderer.cpp \
    Transformer.cpp \
    Types.cpp \
    Util.cpp \
    Widgets/CentralWidget.cpp \
    Widgets/ColorPointWidget.cpp \
    Widgets/ControlPointWidget.cpp \
    Widgets/CurveWidget.cpp \
    Widgets/CustomFlatButton.cpp \
    Widgets/ModeWidget.cpp \
    Widgets/OpenGLWidget.cpp \
    Widgets/RenderSettingsWidget.cpp \
    Widgets/ZoomWidget.cpp \
    Window.cpp

DISTFILES += \
    Shaders/Contour/FragmentShader.frag \
    Shaders/Contour/GeometryShader.geom \
    Shaders/Contour/VertexShader.vert \
    Shaders/Diffusion/Color/LineStrip/FragmentShader.frag \
    Shaders/Diffusion/Color/LineStrip/GeometryShader.geom \
    Shaders/Diffusion/Color/LineStrip/VertexShader.vert \
    Shaders/Diffusion/Color/TriangleStrip/FragmentShader.frag \
    Shaders/Diffusion/Color/TriangleStrip/GeometryShader.geom \
    Shaders/Diffusion/Color/TriangleStrip/VertexShader.vert \
    Shaders/Diffusion/Downsample/FragmentShader.frag \
    Shaders/Diffusion/Downsample/VertexShader.vert \
    Shaders/Diffusion/Jacobi/FragmentShader.frag \
    Shaders/Diffusion/Jacobi/VertexShader.vert \
    Shaders/Diffusion/Upsample/FragmentShader.frag \
    Shaders/Diffusion/Upsample/VertexShader.vert \
    Shaders/Line/FragmentShader.frag \
    Shaders/Line/GeometryShader.geom \
    Shaders/Line/VertexShader.vert \
    Shaders/Point/FragmentShader.frag \
    Shaders/Point/GeometryShader.geom \
    Shaders/Point/VertexShader.vert \
    Shaders/Rectangle/FragmentShader.frag \
    Shaders/Rectangle/GeometryShader.geom \
    Shaders/Rectangle/VertexShader.vert \
    Shaders/Screen/FragmentShader.frag \
    Shaders/Screen/VertexShader.vert

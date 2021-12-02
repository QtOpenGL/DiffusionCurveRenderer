QT += core gui
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Shaders/Bezier/Contour/FragmentShader.frag \
    Shaders/Bezier/Contour/GeometryShader.geom \
    Shaders/Bezier/Contour/VertexShader.vert \
    Shaders/Bezier/Diffusion/FragmentShader.frag \
    Shaders/Bezier/Diffusion/GeometryShader.geom \
    Shaders/Bezier/Diffusion/VertexShader.vert \
    Shaders/Line/FragmentShader.frag \
    Shaders/Line/GeometryShader.geom \
    Shaders/Line/VertexShader.vert \
    Shaders/Point/FragmentShader.frag \
    Shaders/Point/GeometryShader.geom \
    Shaders/Point/VertexShader.vert \
    Shaders/Rectangle/FragmentShader.frag \
    Shaders/Rectangle/GeometryShader.geom \
    Shaders/Rectangle/VertexShader.vert

HEADERS += \
    Constants.h \
    Curves/Bezier.h \
    Curves/ColorPoint.h \
    Curves/ControlPoint.h \
    Curves/Curve.h \
    MainController.h \
    MainWindow.h \
    Renderers/Base/LineRenderer.h \
    Renderers/Base/PointRenderer.h \
    Renderers/Base/RectangleRenderer.h \
    Renderers/Contour/BezierContourRenderer.h \
    Renderers/Diffusion/BezierDiffusionRenderer.h \
    Renderers/GUI/BoundingBoxRenderer.h \
    Renderers/GUI/ControlPointRenderer.h \
    Util.h \
    Widgets/CentralWidget.h \
    Widgets/ControlPointWidget.h \
    Widgets/CurveWidget.h \
    Widgets/LeftWidget.h \
    Widgets/ModeWidget.h \
    Widgets/OpenGLWidget.h \
    Widgets/ZoomWidget.h

SOURCES += \
    Curves/Bezier.cpp \
    Curves/ColorPoint.cpp \
    Curves/ControlPoint.cpp \
    Curves/Curve.cpp \
    Main.cpp \
    MainController.cpp \
    MainWindow.cpp \
    Renderers/Base/LineRenderer.cpp \
    Renderers/Base/PointRenderer.cpp \
    Renderers/Base/RectangleRenderer.cpp \
    Renderers/Contour/BezierContourRenderer.cpp \
    Renderers/Diffusion/BezierDiffusionRenderer.cpp \
    Renderers/GUI/BoundingBoxRenderer.cpp \
    Renderers/GUI/ControlPointRenderer.cpp \
    Util.cpp \
    Widgets/CentralWidget.cpp \
    Widgets/ControlPointWidget.cpp \
    Widgets/CurveWidget.cpp \
    Widgets/LeftWidget.cpp \
    Widgets/ModeWidget.cpp \
    Widgets/OpenGLWidget.cpp \
    Widgets/ZoomWidget.cpp

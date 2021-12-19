#ifndef TYPES_H
#define TYPES_H

typedef int Location;

namespace Constants {
const int MAX_CONTROL_POINT_COUNT = 32;
const int MAX_COLOR_POINT_COUNT = 32;
} // namespace Constants

enum class Action {
    Select,
    AppendControlPoint,
    InsertControlPoint,
    AddColorPoint,
    Move,
    Pan,
    ZoomIn,
    ZoomOut,
    RemoveCurve,
    RemoveControlPoint,
    RemoveColorPoint,
    UpdateControlPointPosition,
    UpdateControlPointXPosition,
    UpdateControlPointYPosition,
    UpdateCurveZIndex,
    UpdateColorPointColor,
    UpdateColorPointPosition,
    UpdateContourThickness,
    UpdateDiffusionWidth,
    UpdateContourColor,
    UpdateSmoothIterations,
    UpdateRenderQuality,
    LoadFromXML,
    SaveAsPNG
};

enum class Mode {
    Select,
    AppendControlPoint,
    InsertControlPoint,
    AddColorPoint,
    MoveCurve,
    Pan,
};

enum class RenderMode { None = 0x01, Contours = 0x02, Diffuse = 0x04 };

enum class DirtType { GUI = 0x01, OpenGL = 0x02 };

struct ProjectionParameters
{
    float left;
    float right;
    float bottom;
    float top;
    float width;
    float height;
    float zoomRatio;
    float pixelRatio;
};

enum class ColorRendererMode { LineStrip, TriangleStrip };

RenderMode operator+(const RenderMode &lhs, const RenderMode &rhs);
bool operator&(const RenderMode &lhs, const RenderMode &rhs);
DirtType operator+(const DirtType &lhs, const DirtType &rhs);
bool operator&(const DirtType &lhs, const DirtType &rhs);

#endif // TYPES_H

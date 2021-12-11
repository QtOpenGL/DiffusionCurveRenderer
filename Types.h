#ifndef TYPES_H
#define TYPES_H

namespace Constants {
const int MAX_CONTROL_POINT_COUNT = 32;
const int MAX_COLOR_POINT_COUNT = 32;
} // namespace Constants

enum class Action {
    Select,
    Add,
    Move,
    Pan,
    ZoomIn,
    ZoomOut,
    RemoveCurve,
    RemoveControlPoint,
    UpdateControlPointPosition,
    UpdateCurveZIndex,
    UpdateContourThickness,
    UpdateDiffusionWidth,
    UpdateContourColor,
    EnableContourColor
};

enum class Mode {
    Select,
    AddControlPoint,
    AddColorPoint,
    MoveCurve,
    Pan,
};

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

#endif // TYPES_H

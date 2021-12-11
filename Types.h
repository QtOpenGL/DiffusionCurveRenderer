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
    UpdateCurveThickness
};

enum class Mode {
    Select,
    Add,
    Move,
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

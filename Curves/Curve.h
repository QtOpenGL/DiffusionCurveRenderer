#ifndef CURVE_H
#define CURVE_H

#include "ColorPoint.h"
#include "ControlPoint.h"
#include "Types.h"

#include <QVector4D>
#include <QVector>

class Curve
{
public:
    Curve();
    virtual ~Curve();

    enum ColorDirection { Left = 0, Right = 1 };

    const float &thickness() const;
    void setThickness(float newThickness);

    const bool &selected() const;
    void setSelected(bool newSelected);

    virtual QVector<const ControlPoint *> getControlPoints() const;
    virtual const ControlPoint *getControlPoint(int index) const;
    virtual QVector<QVector2D> getControlPointPositions() const;
    virtual void addControlPoint(ControlPoint *controlPoint);
    virtual void setControlPoint(int index, ControlPoint *controlPoint);
    virtual void removeControlPoint(int index);
    virtual void removeControlPoint(ControlPoint *controlPoint);

    virtual int getSize() const;
    virtual int getOrder() const;
    virtual int getDegree() const;

    virtual QVector<const ColorPoint *> getColorPoints(ColorDirection direction) const;
    virtual void addColorPoint(ColorDirection direction, ColorPoint *colorPoint);
    virtual const ColorPoint *getColorPoint(ColorDirection direction, int index) const;
    virtual void removeColorPoint(ColorDirection direction, int index);
    virtual void removeColorPoint(ColorDirection direction, ColorPoint *controlPoint);
    virtual QVector4D colorAt(ColorDirection direction, float t) const;
    virtual void orderColorPoints(ColorDirection direction);

    virtual QVector<QVector4D> getColors(ColorDirection direction, bool onlyEnabledColorPoints = true) const;
    virtual QVector<float> getColorPositions(ColorDirection direction, bool onlyEnabledColorPoints = true) const;

    virtual const ControlPoint *getClosestControlPoint(QVector2D point) const;
    virtual void deselectAllControlPoints();
    virtual float distanceToPoint(QVector2D point) const;

    virtual void translate(QVector2D translation);
    virtual void translate(QPointF translation);

    virtual float length(int intervals = 100) const;
    virtual QRectF getBoundingBox(int intervals = 100) const;

    virtual QVector2D valueAt(float t) const = 0;
    virtual QVector2D tangentAt(float t) const = 0;
    virtual QVector2D normalAt(float t) const = 0;

    const int &z() const;
    void setZ(int newZ);

    const QVector4D &curveColor() const;
    void setCurveColor(const QVector4D &newCurveColor);

    const bool &showContour() const;
    void setShowContour(bool newShowContour);

protected:
    QVector<ControlPoint *> mControlPoints;

    QVector<ColorPoint *> mLeftColorPoints;
    QVector<ColorPoint *> mRightColorPoints;

    QVector4D mCurveColor;
    bool mShowContour;

    float mThickness;
    bool mSelected;
    int mZ;

private:
    void updateControlPointIndices();
};

#endif // CURVE_H

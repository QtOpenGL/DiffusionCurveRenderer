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

    virtual QVector<const ColorPoint *> getLeftColorPoints() const;
    virtual QVector<const ColorPoint *> getRightColorPoints() const;

    virtual void addColorPoint(ColorPoint *colorPoint);

    virtual const ColorPoint *getLeftColorPoint(int index) const;
    virtual const ColorPoint *getRightColorPoint(int index) const;

    virtual void removeLeftColorPoint(int index);
    virtual void removeRightColorPoint(int index);
    virtual void removeColorPoint(ColorPoint *controlPoint);

    virtual QVector4D leftColorAt(float t) const;
    virtual QVector4D rightColorAt(float t) const;

    virtual void orderLeftColorPoints();
    virtual void orderRightColorPoints();

    virtual QVector<QVector4D> getLeftColors() const;
    virtual QVector<QVector4D> getRightColors() const;

    virtual QVector<float> getLeftColorPositions() const;
    virtual QVector<float> getRightColorPositions() const;

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

    const bool &selected() const;
    void setSelected(bool newSelected);

    const int &z() const;
    void setZ(int newZ);

    const QVector4D &contourColor() const;
    void setContourColor(const QVector4D &newContourColor);

    bool contourColorEnabled() const;
    void setContourColorEnabled(bool newContourColorEnabled);

    float contourThickness() const;
    void setContourThickness(float newContourThickness);

    float diffusionWidth() const;
    void setDiffusionWidth(float newDiffusionWidth);

protected:
    QVector<ControlPoint *> mControlPoints;

    QVector<ColorPoint *> mLeftColorPoints;
    QVector<ColorPoint *> mRightColorPoints;

    bool mContourColorEnabled;
    QVector4D mContourColor;

    float mContourThickness;
    float mDiffusionWidth;
    bool mSelected;
    int mZ;

private:
    void updateControlPointIndices();
};

#endif // CURVE_H

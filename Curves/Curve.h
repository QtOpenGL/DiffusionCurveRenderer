#ifndef CURVE_H
#define CURVE_H

#include "Constants.h"
#include "ColorPoint.h"
#include "ControlPoint.h"

#include <QObject>
#include <QVector>
#include <QVector4D>
#include <QOpenGLShaderProgram>

class Curve
{
public:
    Curve();
    virtual ~Curve();

    enum ColorDirection {
        Left = 0,
        Right = 1
    };

    float thickness() const;
    void setThickness(float newThickness);

    bool selected() const;
    void setSelected(bool newSelected);

    virtual QVector<ControlPoint*> getControlPoints();
    virtual ControlPoint* getControlPoint(int index);
    virtual QVector<QVector2D> getControlPointPositions();
    virtual void addControlPoint(ControlPoint* controlPoint);
    virtual void setControlPoint(int index, ControlPoint* controlPoint);
    virtual void removeControlPoint(int index);
    virtual void removeControlPoint(ControlPoint* controlPoint);

    virtual int getSize();
    virtual int getOrder();
    virtual int getDegree();

    virtual QVector<ColorPoint*> getColorPoints(ColorDirection direction);
    virtual void addColorPoint(ColorDirection direction, ColorPoint* colorPoint);
    virtual ColorPoint* getColorPoint(ColorDirection direction, int index);
    virtual void removeColorPoint(ColorDirection direction, int index);
    virtual void removeColorPoint(ColorDirection direction, ColorPoint* controlPoint);
    virtual QVector4D colorAt(ColorDirection direction, float t);
    virtual void orderColorPoints(ColorDirection direction);

    virtual QVector<QVector4D> getColors(ColorDirection direction, bool onlyEnabledColorPoints = true);
    virtual QVector<GLfloat> getColorPositions(ColorDirection direction, bool onlyEnabledColorPoints = true);

    virtual ControlPoint* getClosestControlPoint(QPointF point, float radius = 12);
    virtual void deselectAllControlPoints();
    virtual float distanceToPoint(QPointF point);

    virtual void translate(QVector2D translation);
    virtual void translate(QPointF translation);

    virtual float length(int intervals = 100);
    virtual QRectF getBoundingBox(float space);

    virtual QVector2D valueAt(float t) = 0;
    virtual QVector2D tangentAt(float t) = 0;
    virtual QVector2D normalAt(float t) = 0;

    int z() const;
    void setZ(int newZ);

protected:
    QVector<ControlPoint*> mControlPoints;
    QVector<ColorPoint*> mLeftColorPoints, mRightColorPoints;
    float mThickness;
    bool mSelected;
    int mZ;

private:
    void updateControlPointIndices();
};

#endif // CURVE_H

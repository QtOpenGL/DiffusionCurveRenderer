#ifndef BEZIER_H
#define BEZIER_H

#include "ControlPoint.h"
#include "Curve.h"

#include <QRectF>
#include <QVector2D>
#include <QVector4D>
#include <QVector>

class Bezier : public Curve
{
public:
    Bezier();
    ~Bezier();

    QVector2D valueAt(float t) override;
    QVector2D tangentAt(float t) override;
    QVector2D normalAt(float t) override;

private:
    QVector<float> getCoefficients();
    QVector<float> getDerivativeCoefficients();
    float factorial(int n);
    float choose(int n, int k);
};

#endif // BEZIER_H

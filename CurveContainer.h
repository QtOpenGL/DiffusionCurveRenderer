#ifndef CURVECONTAINER_H
#define CURVECONTAINER_H

#include <Curves/Curve.h>

class CurveContainer : public QObject
{
    Q_OBJECT
public:
    explicit CurveContainer(QObject *parent = nullptr);
    ~CurveContainer();

    void addCurve(Curve *curve);
    void addCurves(const QVector<Curve *> curves);

    QVector<Curve *> &getCurves();

    Curve *selectedCurve();
    void setSelectedCurve(Curve *selectedCurve);

    ControlPoint *selectedControlPoint();
    void setSelectedControlPoint(ControlPoint *selectedControlPoint);

    void removeCurve(int index);
    void removeCurve(Curve *curve);

    void deselectAllCurves();

    Curve *selectCurve(QVector2D position, float radius = 20.0f);
    ControlPoint *getClosestControlPointOnSelectedCurve(QVector2D position, float radius = 20.0f);

signals:
    void selectedCurveChanged(Curve *selectedCurve);
    void selectedControlPointChanged(ControlPoint *selectedControlPoint);

private:
    QVector<Curve *> mCurves;
    Curve *mSelectedCurve;
    ControlPoint *mSelectedControlPoint;
};

#endif // CURVECONTAINER_H

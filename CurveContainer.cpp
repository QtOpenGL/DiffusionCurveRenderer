#include "CurveContainer.h"

CurveContainer::CurveContainer(QObject *parent)
    : QObject(parent)
    , mSelectedCurve(nullptr)
    , mSelectedControlPoint(nullptr)
{}

CurveContainer::~CurveContainer()
{
    for (auto &curve : mCurves) {
        if (curve)
            delete curve;

        curve = nullptr;
    }
}

void CurveContainer::addCurve(Curve *curve)
{
    mCurves << curve;
}

void CurveContainer::addCurves(const QVector<Curve *> curves)
{
    mCurves << curves;
}

QVector<Curve *> &CurveContainer::getCurves()
{
    return mCurves;
}

ControlPoint *CurveContainer::getClosestControlPointOnSelectedCurve(QVector2D position, float radius)
{
    if (!mSelectedCurve)
        return nullptr;

    ControlPoint *controlPoint = mSelectedCurve->getClosestControlPoint(position);

    if (controlPoint) {
        if (controlPoint->position.distanceToPoint(position) > radius)
            controlPoint = nullptr;
    }

    return controlPoint;
}

Curve *CurveContainer::selectedCurve()
{
    return mSelectedCurve;
}

ControlPoint *CurveContainer::selectedControlPoint()
{
    return mSelectedControlPoint;
}

void CurveContainer::removeCurve(int index)
{
    if (0 <= index && index < mCurves.size()) {
        Curve *curve = mCurves[index];
        mCurves.removeAt(index);
        delete curve;
    }
}

void CurveContainer::removeCurve(Curve *curve)
{
    for (int i = 0; i < mCurves.size(); ++i) {
        if (mCurves[i] == curve) {
            removeCurve(i);
            return;
        }
    }
}

Curve *CurveContainer::selectCurve(QVector2D position, float radius)
{
    float minDistance = std::numeric_limits<float>::infinity();

    Curve *selectedCurve = nullptr;

    for (int i = 0; i < mCurves.size(); ++i) {
        float distance = mCurves[i]->distanceToPoint(position);
        if (distance < minDistance) {
            minDistance = distance;
            selectedCurve = mCurves[i];
        }
    }

    if (minDistance < radius)
        return selectedCurve;

    return nullptr;
}

void CurveContainer::deselectAllCurves()
{
    for (int i = 0; i < mCurves.size(); ++i)
        mCurves[i]->setSelected(false);

    mSelectedCurve = nullptr;
}

void CurveContainer::setSelectedCurve(Curve *selectedCurve)
{
    if (mSelectedCurve == selectedCurve)
        return;

    if (mSelectedCurve)
        mSelectedCurve->setSelected(false);

    if (selectedCurve)
        selectedCurve->setSelected(true);

    mSelectedCurve = selectedCurve;
    emit selectedCurveChanged(selectedCurve);
}

void CurveContainer::setSelectedControlPoint(ControlPoint *selectedControlPoint)
{
    if (mSelectedControlPoint == selectedControlPoint)
        return;

    if (mSelectedControlPoint)
        mSelectedControlPoint->selected = false;

    if (selectedControlPoint)
        selectedControlPoint->selected = true;

    mSelectedControlPoint = selectedControlPoint;
    emit selectedControlPointChanged(selectedControlPoint);
}
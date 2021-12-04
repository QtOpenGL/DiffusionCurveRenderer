#ifndef CONTROLPOINTWIDGET_H
#define CONTROLPOINTWIDGET_H

#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>

#include "Curves/ControlPoint.h"
#include "Curves/Curve.h"
#include "Enums.h"

class ControlPointWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ControlPointWidget(QGroupBox *parent = nullptr);

signals:
    void dirty();
    void selectedControlPointChanged(ControlPoint *selectedControlPoint);
    void action(Action action);

public slots:
    void onSelectedControlPointChanged(ControlPoint *selectedControlPoint);
    void onSelectedCurveChanged(Curve *selectedCurve);
    void onDirty();

private:
    QComboBox *mControlPointComboBox;
    QLineEdit *mControlPointX;
    QLineEdit *mControlPointY;
    QPushButton *mRemoveButton;

    Curve *mSelectedCurve;
    ControlPoint *mSelectedControlPoint;
};

#endif // CONTROLPOINTWIDGET_H

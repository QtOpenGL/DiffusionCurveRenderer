#ifndef CONTROLPOINTWIDGET_H
#define CONTROLPOINTWIDGET_H

#include <CustomVariant.h>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "Curves/ControlPoint.h"
#include "Curves/Curve.h"
#include "Types.h"

class ControlPointWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ControlPointWidget(QGroupBox *parent = nullptr);

signals:
    void action(Action action, CustomVariant value = CustomVariant());

public slots:
    void onSelectedControlPointChanged(ControlPoint *selectedControlPoint);
    void onSelectedCurveChanged(Curve *selectedCurve);
    void refresh();

private:
    QLabel *mControlPointLabel;
    QLineEdit *mControlPointX;
    QLineEdit *mControlPointY;
    QPushButton *mRemoveButton;

    const Curve *mSelectedCurve;
    const ControlPoint *mSelectedControlPoint;
};

#endif // CONTROLPOINTWIDGET_H

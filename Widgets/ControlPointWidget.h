#ifndef CONTROLPOINTWIDGET_H
#define CONTROLPOINTWIDGET_H

#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include "Curves/Curve.h"
#include "Curves/ControlPoint.h"

class ControlPointWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ControlPointWidget(QGroupBox *parent = nullptr);

signals:
    void dirty();
    void selectedControlPointChanged(ControlPoint* selectedControlPoint);
    void removeControlPointButtonClicked();

public slots:
    void onSelectedControlPointChanged(ControlPoint* selectedControlPoint);
    void onSelectedCurveChanged(Curve* selectedCurve);
    void onDirty();

private:
    QComboBox* mControlPointComboBox;
    QLineEdit* mControlPointX;
    QLineEdit* mControlPointY;
    QPushButton* mRemoveButton;

    Curve* mSelectedCurve;
    ControlPoint* mSelectedControlPoint;

};

#endif // CONTROLPOINTWIDGET_H

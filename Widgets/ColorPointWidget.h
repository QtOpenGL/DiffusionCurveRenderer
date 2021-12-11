#ifndef COLORPOINTWIDGET_H
#define COLORPOINTWIDGET_H

#include "CustomFlatButton.h"

#include <CustomVariant.h>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>

#include <Curves/Curve.h>

class ColorPointWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ColorPointWidget(QWidget *parent = nullptr);

signals:
    void action(Action action, CustomVariant value = CustomVariant());

public slots:
    void onSelectedColorPointChanged(ColorPoint *selectedColorPoint);
    void onSelectedCurveChanged(Curve *selectedCurve);
    void refresh();

private:
    QLabel *mTypeLabel;
    QSlider *mPositionSlider;
    CustomFlatButton *mColorButton;
    QPushButton *mRemoveButton;

    const Curve *mSelectedCurve;
    const ColorPoint *mSelectedColorPoint;
};

#endif // COLORPOINTWIDGET_H

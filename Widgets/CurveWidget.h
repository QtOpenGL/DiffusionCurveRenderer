#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "Curves/Curve.h"
#include "Types.h"

class CurveWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit CurveWidget(QGroupBox *parent = nullptr);

public slots:
    void onSelectedCurveChanged(Curve *selectedCurve);
    void reset();

signals:
    void action(Action action, QVariant value = QVariant());

private:
    QLineEdit *mZLineEdit;
    QSlider *mContourThicknessSlider;
    QSlider *mDiffusionWidthSlider;
    QCheckBox *mContourColorStateCheckBox;
    QPushButton *mColorButton;
    QPushButton *mRemoveButton;

    const Curve *mSelectedCurve;
};

#endif // CURVEWIDGET_H

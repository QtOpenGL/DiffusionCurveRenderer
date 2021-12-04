#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "Curves/Curve.h"
#include "Enums.h"

class CurveWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit CurveWidget(QGroupBox *parent = nullptr);

public slots:
    void onSelectedCurveChanged(Curve *selectedCurve);
    void reset();

signals:
    void dirty();
    void action(Action action);

private:
    QLineEdit *mZLineEdit;
    QLineEdit *mThicknessLineEdit;
    QPushButton *mRemoveButton;

    Curve *mSelectedCurve;
};

#endif // CURVEWIDGET_H

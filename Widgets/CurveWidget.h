#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>

#include "Curves/Curve.h"

class CurveWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit CurveWidget(QGroupBox *parent = nullptr);

public slots:
    void onSelectedCurveChanged(Curve* selectedCurve);
    void reset();

signals:
    void dirty();
    void removeCurveButtonClicked();

private:
    QLineEdit* mZLineEdit;
    QLineEdit* mThicknessLineEdit;
    QPushButton* mRemoveButton;

    Curve* mSelectedCurve;

};

#endif // CURVEWIDGET_H

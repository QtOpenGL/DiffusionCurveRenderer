#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <CustomVariant.h>
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
    void onDirty(DirtType);

signals:
    void action(Action action, CustomVariant value = CustomVariant());

private:
    QLineEdit *mZLineEdit;
    QPushButton *mRemoveButton;
    const Curve *mSelectedCurve;
};

#endif // CURVEWIDGET_H

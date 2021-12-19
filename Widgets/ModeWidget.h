#ifndef MODEWIDGET_H
#define MODEWIDGET_H

#include <QGroupBox>
#include <QRadioButton>

#include <Curves/Curve.h>

#include "Types.h"

class ModeWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ModeWidget(QGroupBox *parent = nullptr);

public slots:
    void onSelectedCurveChanged(Curve *selectedCurve);
    void onDirty(DirtType);
    void onModeChanged(Mode mode);

private slots:
    void refresh();

signals:
    void modeChanged(Mode mode);

private:
    QVector<QRadioButton *> mRadioButtons;
    Mode mMode;

    const Curve *mSelectedCurve;
    int mAddColorPointRadioButtonIndex;
};

#endif // MODEWIDGET_H

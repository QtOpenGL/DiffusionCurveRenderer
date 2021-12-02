#ifndef LEFTWIDGET_H
#define LEFTWIDGET_H

#include "ControlPointWidget.h"
#include "CurveWidget.h"
#include "ModeWidget.h"
#include "ZoomWidget.h"
#include <QWidget>

class LeftWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LeftWidget(QWidget *parent = nullptr);

    ModeWidget *modeWidget() const;
    CurveWidget *curveWidget() const;
    ControlPointWidget *controlPointWidget() const;
    ZoomWidget *zoomWidget() const;

signals:
    void zoomRatioChanged(int zoomRatio);
    void showContoursStateChanged(bool state);

private:
    ModeWidget *mModeWidget;
    CurveWidget *mCurveWidget;
    ControlPointWidget *mControlPointWidget;
    ZoomWidget *mZoomWidget;
    QCheckBox *mShowContoursCheckBox;
};

#endif // LEFTWIDGET_H

#include "LeftWidget.h"

#include <QHBoxLayout>

LeftWidget::LeftWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mModeWidget = new ModeWidget;
    mainLayout->addWidget(mModeWidget);

    mCurveWidget = new CurveWidget;
    mainLayout->addWidget(mCurveWidget);

    mControlPointWidget = new ControlPointWidget;
    mainLayout->addWidget(mControlPointWidget);

    mZoomWidget = new ZoomWidget;
    mainLayout->addWidget(mZoomWidget);

    mainLayout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Ignored, QSizePolicy::Expanding));

    setFixedWidth(300);
    setLayout(mainLayout);
}

ModeWidget *LeftWidget::modeWidget() const
{
    return mModeWidget;
}

CurveWidget *LeftWidget::curveWidget() const
{
    return mCurveWidget;
}

ControlPointWidget *LeftWidget::controlPointWidget() const
{
    return mControlPointWidget;
}

ZoomWidget *LeftWidget::zoomWidget() const
{
    return mZoomWidget;
}

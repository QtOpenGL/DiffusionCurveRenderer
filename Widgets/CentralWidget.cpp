#include "CentralWidget.h"

#include "ControlPointWidget.h"
#include "CurveWidget.h"
#include "ModeWidget.h"
#include "OpenGLWidget.h"
#include "ZoomWidget.h"
#include <QGridLayout>

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
{}

void CentralWidget::initialize()
{
    QGridLayout *gridLayout = new QGridLayout;

    gridLayout->addWidget(mModeWidget, 0, 0);
    gridLayout->addWidget(mCurveWidget, 1, 0);
    gridLayout->addWidget(mControlPointWidget, 2, 0);
    gridLayout->addWidget(mZoomWidget, 3, 0);
    gridLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Ignored, QSizePolicy::Expanding), 4, 0);

    mShowContoursCheckBox = new QCheckBox("Show Contours");
    mShowContoursCheckBox->setCheckable(true);
    mShowContoursCheckBox->setChecked(true);
    gridLayout->addWidget(mShowContoursCheckBox, 5, 0);
    connect(mShowContoursCheckBox, &QCheckBox::stateChanged, this, [this](int state) {
        emit showContoursStateChanged(state == 2);
    });

    QHBoxLayout *layout = new QHBoxLayout;
    mOpenGLWidgetContainer = new QGroupBox;
    mOpenGLWidgetContainer->setTitle("Canvas");
    mOpenGLWidgetContainer->setLayout(layout);
    layout->addWidget(mOpenGLWidget);
    gridLayout->addWidget(mOpenGLWidgetContainer, 0, 1, 0, -1);
    gridLayout->setColumnStretch(1, 1);

    setLayout(gridLayout);
}

void CentralWidget::setOpenGLWidget(OpenGLWidget *newOpenGLWidget) { mOpenGLWidget = newOpenGLWidget; }

void CentralWidget::setModeWidget(ModeWidget *newModeWidget) { mModeWidget = newModeWidget; }

void CentralWidget::setZoomWidget(ZoomWidget *newZoomWidget) { mZoomWidget = newZoomWidget; }

void CentralWidget::setCurveWidget(CurveWidget *newCurveWidget) { mCurveWidget = newCurveWidget; }

void CentralWidget::setControlPointWidget(ControlPointWidget *newControlPointWidget)
{
    mControlPointWidget = newControlPointWidget;
}

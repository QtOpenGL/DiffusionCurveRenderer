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

void CentralWidget::init()
{
    mClearButton = new QPushButton("Clear Canvas");
    connect(mClearButton, &QPushButton::clicked, this, [=]() { emit action(Action::ClearCanvas); });

    // OpenGL Widget and Container
    QHBoxLayout *layout = new QHBoxLayout;
    mOpenGLWidgetContainer = new QGroupBox;
    mOpenGLWidgetContainer->setTitle("Canvas");
    mOpenGLWidgetContainer->setLayout(layout);
    layout->addWidget(mOpenGLWidget);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(mModeWidget, 0, 0);
    gridLayout->addWidget(mCurveWidget, 1, 0);
    gridLayout->addWidget(mControlPointWidget, 2, 0);
    gridLayout->addWidget(mColorPointWidget, 3, 0);
    gridLayout->addWidget(mRenderSettingsWidget, 4, 0);
    gridLayout->addWidget(mZoomWidget, 5, 0);
    gridLayout->addWidget(mClearButton, 6, 0);
    gridLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Ignored, QSizePolicy::Expanding), 7, 0);
    gridLayout->addWidget(mOpenGLWidgetContainer, 0, 1, 0, -1);
    gridLayout->setColumnStretch(1, 1);

    mZoomWidget->setMinimumWidth(300);
    setLayout(gridLayout);
}

void CentralWidget::setOpenGLWidget(OpenGLWidget *newOpenGLWidget)
{
    mOpenGLWidget = newOpenGLWidget;
}

void CentralWidget::setModeWidget(ModeWidget *newModeWidget)
{
    mModeWidget = newModeWidget;
}

void CentralWidget::setZoomWidget(ZoomWidget *newZoomWidget)
{
    mZoomWidget = newZoomWidget;
}

void CentralWidget::setCurveWidget(CurveWidget *newCurveWidget)
{
    mCurveWidget = newCurveWidget;
}

void CentralWidget::setColorPointWidget(ColorPointWidget *newColorPointWidget)
{
    mColorPointWidget = newColorPointWidget;
}

void CentralWidget::setRenderSettingsWidget(RenderSettingsWidget *newRenderSettingsWidget)
{
    mRenderSettingsWidget = newRenderSettingsWidget;
}

void CentralWidget::onKeyPressed(Qt::Key key)
{
    emit keyPressed(key);
}

void CentralWidget::onKeyReleased(Qt::Key key)
{
    emit keyReleased(key);
}

void CentralWidget::setControlPointWidget(ControlPointWidget *newControlPointWidget)
{
    mControlPointWidget = newControlPointWidget;
}

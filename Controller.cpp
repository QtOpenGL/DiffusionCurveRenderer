#include "Controller.h"

#include <Widgets/CentralWidget.h>
#include <Widgets/ControlPointWidget.h>
#include <Widgets/CurveWidget.h>
#include <Widgets/ModeWidget.h>
#include <Widgets/OpenGLWidget.h>
#include <Widgets/ZoomWidget.h>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    mCentralWidget = new CentralWidget;
    mOpenGLWidget = new OpenGLWidget;
    mModeWidget = new ModeWidget;
    mCurveWidget = new CurveWidget;
    mControlPointWidget = new ControlPointWidget;
    mZoomWidget = new ZoomWidget;
    mCurveContainer = new CurveContainer;

    mOpenGLWidget->setCurveContainer(mCurveContainer);

    mCentralWidget->setOpenGLWidget(mOpenGLWidget);
    mCentralWidget->setModeWidget(mModeWidget);
    mCentralWidget->setCurveWidget(mCurveWidget);
    mCentralWidget->setControlPointWidget(mControlPointWidget);
    mCentralWidget->setZoomWidget(mZoomWidget);

    mCentralWidget->initialize();

    connect(mCurveContainer, &CurveContainer::selectedCurveChanged, mCurveWidget, &CurveWidget::onSelectedCurveChanged);
    connect(mCurveContainer, &CurveContainer::selectedCurveChanged, mControlPointWidget, &ControlPointWidget::onSelectedCurveChanged);
    connect(mCurveContainer, &CurveContainer::selectedControlPointChanged, mControlPointWidget, &ControlPointWidget::onSelectedControlPointChanged);

    connect(mOpenGLWidget, &OpenGLWidget::dirty, mControlPointWidget, &ControlPointWidget::onDirty);
    connect(mOpenGLWidget, &OpenGLWidget::zoomRatioChanged, mZoomWidget, &ZoomWidget::onZoomRatioChanged);

    connect(mZoomWidget, &ZoomWidget::zoomRatioChanged, mOpenGLWidget, &OpenGLWidget::onZoomRatioChanged);

    connect(mCurveWidget, &CurveWidget::dirty, mOpenGLWidget, &OpenGLWidget::onDirty);
    connect(mCurveWidget, &CurveWidget::action, mOpenGLWidget, &OpenGLWidget::onAction);
    connect(mCurveWidget, &CurveWidget::action, mCurveContainer, &CurveContainer::sortCurves);

    connect(mControlPointWidget, &ControlPointWidget::dirty, mOpenGLWidget, &OpenGLWidget::onDirty);
    connect(mControlPointWidget, &ControlPointWidget::selectedControlPointChanged, mOpenGLWidget, &OpenGLWidget::onSelectedControlPointChanged);
    connect(mControlPointWidget, &ControlPointWidget::action, mOpenGLWidget, &OpenGLWidget::onAction);

    connect(mModeWidget, &ModeWidget::modeChanged, mOpenGLWidget, &OpenGLWidget::onModeChanged);

    connect(mCentralWidget, &CentralWidget::showContoursStateChanged, mOpenGLWidget, &OpenGLWidget::onShowContoursStateChanged);
}

CentralWidget *Controller::centralWidget() const
{
    return mCentralWidget;
}

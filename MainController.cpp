#include "MainController.h"

#include <Widgets/CentralWidget.h>
#include <Widgets/ControlPointWidget.h>
#include <Widgets/CurveWidget.h>
#include <Widgets/ModeWidget.h>
#include <Widgets/OpenGLWidget.h>
#include <Widgets/ZoomWidget.h>

MainController::MainController(QObject *parent)
    : QObject(parent)
{
    mCentralWidget = new CentralWidget;
    mOpenGLWidget = new OpenGLWidget;
    mModeWidget = new ModeWidget;
    mCurveWidget = new CurveWidget;
    mControlPointWidget = new ControlPointWidget;
    mZoomWidget = new ZoomWidget;

    mCentralWidget->setOpenGLWidget(mOpenGLWidget);
    mCentralWidget->setModeWidget(mModeWidget);
    mCentralWidget->setCurveWidget(mCurveWidget);
    mCentralWidget->setControlPointWidget(mControlPointWidget);
    mCentralWidget->setZoomWidget(mZoomWidget);

    mCentralWidget->initialize();

    connect(mOpenGLWidget, &OpenGLWidget::selectedCurveChanged, mCurveWidget, &CurveWidget::onSelectedCurveChanged);
    connect(mOpenGLWidget,
            &OpenGLWidget::selectedCurveChanged,
            mControlPointWidget,
            &ControlPointWidget::onSelectedCurveChanged);
    connect(mOpenGLWidget,
            &OpenGLWidget::selectedControlPointChanged,
            mControlPointWidget,
            &ControlPointWidget::onSelectedControlPointChanged);
    connect(mOpenGLWidget, &OpenGLWidget::dirty, mControlPointWidget, &ControlPointWidget::onDirty);
    connect(mOpenGLWidget, &OpenGLWidget::zoomRatioChanged, mZoomWidget, &ZoomWidget::onZoomRatioChanged);

    connect(mZoomWidget, &ZoomWidget::zoomRatioChanged, mOpenGLWidget, &OpenGLWidget::onZoomRatioChanged);

    connect(mCurveWidget, &CurveWidget::dirty, mOpenGLWidget, &OpenGLWidget::onDirty);
    connect(mCurveWidget, &CurveWidget::removeCurveButtonClicked, this, [=]() {
        mOpenGLWidget->onAction(OpenGLWidget::Action::RemoveCurve);
    });

    connect(mControlPointWidget, &ControlPointWidget::dirty, mOpenGLWidget, &OpenGLWidget::onDirty);
    connect(mControlPointWidget,
            &ControlPointWidget::selectedControlPointChanged,
            mOpenGLWidget,
            &OpenGLWidget::onSelectedControlPointChanged);
    connect(mControlPointWidget, &ControlPointWidget::removeControlPointButtonClicked, this, [=]() {
        mOpenGLWidget->onAction(OpenGLWidget::Action::RemoveControlPoint);
    });

    connect(mModeWidget, &ModeWidget::modeChanged, mOpenGLWidget, &OpenGLWidget::onModeChanged);

    connect(mCentralWidget,
            &CentralWidget::showContoursStateChanged,
            mOpenGLWidget,
            &OpenGLWidget::onShowContoursStateChanged);
}

CentralWidget *MainController::centralWidget() const { return mCentralWidget; }

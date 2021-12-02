#include "CentralWidget.h"

#include <QHBoxLayout>
#include <QMenuBar>

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;

    // Left Widget and OpenGLWidget
    {
        mLeftWidget = new LeftWidget;
        mainLayout->addWidget(mLeftWidget);

        QHBoxLayout *layout = new QHBoxLayout;
        mGLWidgetContainer = new QGroupBox;
        mGLWidgetContainer->setTitle("Canvas");
        mGLWidgetContainer->setLayout(layout);

        mGLWidget = new OpenGLWidget;
        layout->addWidget(mGLWidget);
        mainLayout->addWidget(mGLWidgetContainer);

        // Connections
        connect(mGLWidget,
                &OpenGLWidget::selectedCurveChanged,
                mLeftWidget->curveWidget(),
                &CurveWidget::onSelectedCurveChanged);

        connect(mGLWidget,
                &OpenGLWidget::selectedCurveChanged,
                mLeftWidget->controlPointWidget(),
                &ControlPointWidget::onSelectedCurveChanged);

        connect(mGLWidget,
                &OpenGLWidget::selectedControlPointChanged,
                mLeftWidget->controlPointWidget(),
                &ControlPointWidget::onSelectedControlPointChanged);

        connect(mGLWidget, &OpenGLWidget::dirty, mLeftWidget->controlPointWidget(), &ControlPointWidget::onDirty);
        connect(mGLWidget, &OpenGLWidget::zoomRatioChanged, mLeftWidget->zoomWidget(), &ZoomWidget::onZoomRatioChanged);

        connect(mLeftWidget->zoomWidget(), &ZoomWidget::zoomRatioChanged, mGLWidget, &OpenGLWidget::onZoomRatioChanged);
        connect(mLeftWidget->curveWidget(), &CurveWidget::dirty, mGLWidget, &OpenGLWidget::onDirty);
        connect(mLeftWidget->curveWidget(), &CurveWidget::removeCurveButtonClicked, this, [=]() {
            mGLWidget->onAction(OpenGLWidget::Action::RemoveCurve);
        });

        connect(mLeftWidget->controlPointWidget(), &ControlPointWidget::dirty, mGLWidget, &OpenGLWidget::onDirty);
        connect(mLeftWidget->controlPointWidget(),
                &ControlPointWidget::selectedControlPointChanged,
                mGLWidget,
                &OpenGLWidget::onSelectedControlPointChanged);
        connect(mLeftWidget->controlPointWidget(), &ControlPointWidget::removeControlPointButtonClicked, this, [=]() {
            mGLWidget->onAction(OpenGLWidget::Action::RemoveControlPoint);
        });

        connect(mLeftWidget->modeWidget(), &ModeWidget::modeChanged, mGLWidget, &OpenGLWidget::onModeChanged);

        connect(mLeftWidget,
                &LeftWidget::showContoursStateChanged,
                mGLWidget,
                &OpenGLWidget::onShowContoursStateChanged);
    }

    setLayout(mainLayout);
}

OpenGLWidget *CentralWidget::gLWidget() const { return mGLWidget; }

void CentralWidget::setGLWidget(OpenGLWidget *newGLWidget) { mGLWidget = newGLWidget; }

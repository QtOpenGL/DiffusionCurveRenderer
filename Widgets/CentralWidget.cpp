#include "CentralWidget.h"

#include <QHBoxLayout>
#include <QMenuBar>

CentralWidget::CentralWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout* mainLayout = new QHBoxLayout;

    // Menu
    {
        QAction* importAction = new QAction("Import");
        connect(importAction, &QAction::triggered, this, [=](){
            qDebug() << importAction;
        });

        QAction* exportAction = new QAction("Export");

        connect(exportAction, &QAction::triggered, this, [=](){
            qDebug() << exportAction;
        });

        QMenuBar* menuBar = new QMenuBar;
        QMenu *fileMenu = new QMenu("File");
        menuBar->addMenu(fileMenu);
        fileMenu->addAction(importAction);
        fileMenu->addAction(exportAction);

        mainLayout->setMenuBar(menuBar);

    }

    // Left Widget and GLWidget
    {
        mLeftWidget = new LeftWidget;
        mainLayout->addWidget(mLeftWidget);

        QHBoxLayout* layout = new QHBoxLayout;
        mGLWidgetContainer = new QGroupBox;
        mGLWidgetContainer->setTitle("Canvas");
        mGLWidgetContainer->setLayout(layout);

        mGLWidget = new GLWidget;
        layout->addWidget(mGLWidget);
        mainLayout->addWidget(mGLWidgetContainer);


        // Connections
        connect(mGLWidget, &GLWidget::selectedCurveChanged, mLeftWidget->curveWidget(), &CurveWidget::onSelectedCurveChanged);
        connect(mGLWidget, &GLWidget::selectedCurveChanged, mLeftWidget->controlPointWidget(), &ControlPointWidget::onSelectedCurveChanged);
        connect(mGLWidget, &GLWidget::selectedControlPointChanged, mLeftWidget->controlPointWidget(), &ControlPointWidget::onSelectedControlPointChanged);
        connect(mGLWidget, &GLWidget::dirty, mLeftWidget->controlPointWidget(), &ControlPointWidget::onDirty);
        connect(mGLWidget, &GLWidget::zoomRatioChanged, mLeftWidget->zoomWidget(), &ZoomWidget::onZoomRatioChanged);

        connect(mLeftWidget->zoomWidget(), &ZoomWidget::zoomRatioChanged, mGLWidget, &GLWidget::onZoomRatioChanged);
        connect(mLeftWidget->curveWidget(), &CurveWidget::dirty, mGLWidget, &GLWidget::onDirty);
        connect(mLeftWidget->curveWidget(), &CurveWidget::removeCurveButtonClicked, this, [=](){
            mGLWidget->onAction(GLWidget::Action::RemoveCurve);
        });

        connect(mLeftWidget->controlPointWidget(), &ControlPointWidget::dirty, mGLWidget, &GLWidget::onDirty);
        connect(mLeftWidget->controlPointWidget(), &ControlPointWidget::selectedControlPointChanged, mGLWidget, &GLWidget::onSelectedControlPointChanged);
        connect(mLeftWidget->controlPointWidget(), &ControlPointWidget::removeControlPointButtonClicked, this, [=](){
            mGLWidget->onAction(GLWidget::Action::RemoveControlPoint);
        });

        connect(mLeftWidget->modeWidget(), &ModeWidget::modeChanged, mGLWidget, &GLWidget::onModeChanged);
    }

    setLayout(mainLayout);
}

GLWidget *CentralWidget::gLWidget() const
{
    return mGLWidget;
}

void CentralWidget::setGLWidget(GLWidget *newGLWidget)
{
    mGLWidget = newGLWidget;
}

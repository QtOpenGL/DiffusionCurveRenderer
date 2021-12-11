#include "Controller.h"
#include "Util.h"

#include <Widgets/CentralWidget.h>
#include <Widgets/ControlPointWidget.h>
#include <Widgets/CurveWidget.h>
#include <Widgets/ModeWidget.h>
#include <Widgets/OpenGLWidget.h>
#include <Widgets/ZoomWidget.h>

Controller::Controller(QObject *parent)
    : QObject(parent)
    , mMode(Mode::Select)
    , mMouseRightButtonPressed(false)
    , mMouseLeftButtonPressed(false)
    , mMousePressedOnCurve(false)
    , mZoomStep(2.0f)
{
    mCentralWidget = new CentralWidget;
    mOpenGLWidget = new OpenGLWidget;
    mModeWidget = new ModeWidget;
    mCurveWidget = new CurveWidget;
    mControlPointWidget = new ControlPointWidget;
    mZoomWidget = new ZoomWidget;
    mColorPointWidget = new ColorPointWidget;
    mRenderSettingsWidget = new RenderSettingsWidget;

    mCurveContainer = new CurveContainer;
    mRendererManager = new RendererManager;
    mProjectionParameters = new ProjectionParameters;
    mTransformer = new Transformer;

    mCentralWidget->setOpenGLWidget(mOpenGLWidget);
    mCentralWidget->setModeWidget(mModeWidget);
    mCentralWidget->setCurveWidget(mCurveWidget);
    mCentralWidget->setControlPointWidget(mControlPointWidget);
    mCentralWidget->setZoomWidget(mZoomWidget);
    mCentralWidget->setColorPointWidget(mColorPointWidget);
    mCentralWidget->setRenderSettingsWidget(mRenderSettingsWidget);
    mCentralWidget->init();

    mProjectionParameters->zoomRatio = 1.0f;

    mTransformer->setProjectionParameters(mProjectionParameters);

    mOpenGLWidget->setController(this);
    mOpenGLWidget->setTransformer(mTransformer);
    mOpenGLWidget->setProjectionParameters(mProjectionParameters);

    mRendererManager->setCurveContainer(mCurveContainer);
    mRendererManager->setProjectionParameters(mProjectionParameters);

    connect(mCurveContainer, &CurveContainer::selectedCurveChanged, mCurveWidget, &CurveWidget::onSelectedCurveChanged);
    connect(mCurveContainer, &CurveContainer::selectedCurveChanged, mControlPointWidget, &ControlPointWidget::onSelectedCurveChanged);
    connect(mCurveContainer, &CurveContainer::selectedCurveChanged, mOpenGLWidget, &OpenGLWidget::onSelectedCurveChanged);

    connect(mCurveContainer, &CurveContainer::selectedControlPointChanged, mOpenGLWidget, &OpenGLWidget::onSelectedControlPointChanged);
    connect(mCurveContainer, &CurveContainer::selectedControlPointChanged, mControlPointWidget, &ControlPointWidget::onSelectedControlPointChanged);

    connect(mModeWidget, &ModeWidget::modeChanged, mOpenGLWidget, &OpenGLWidget::onModeChanged);
    connect(mModeWidget, &ModeWidget::modeChanged, this, &Controller::onModeChanged);

    connect(mOpenGLWidget, &OpenGLWidget::wheelMoved, this, &Controller::onWheelMoved);
    connect(mOpenGLWidget, &OpenGLWidget::mousePressed, this, &Controller::onMousePressed);
    connect(mOpenGLWidget, &OpenGLWidget::mouseReleased, this, &Controller::onMouseReleased);
    connect(mOpenGLWidget, &OpenGLWidget::mouseMoved, this, &Controller::onMouseMoved);

    connect(mCurveWidget, &CurveWidget::action, this, &Controller::onAction);
    connect(mControlPointWidget, &ControlPointWidget::action, this, &Controller::onAction);

    connect(this, &Controller::zoomRatioChanged, mZoomWidget, &ZoomWidget::onZoomRatioChanged);
    connect(mZoomWidget, &ZoomWidget::zoomRatioChanged, this, &Controller::onZoomRatioChanged);
}

CentralWidget *Controller::centralWidget() const
{
    return mCentralWidget;
}

void Controller::initializeOpenGL()
{
    Bezier *bezier = new Bezier();
    bezier->addControlPoint(new ControlPoint(50.0f, 250.0f));  // First
    bezier->addControlPoint(new ControlPoint(300.0f, 250.0f)); // Last
    mCurveContainer->addCurve(bezier);

    QVector<Curve *> curves = Util::readCurveDataFromXML("Resources/CurveData/zephyr.xml");
    mCurveContainer->addCurves(curves);

    mProjectionParameters->left = 0;
    mProjectionParameters->right = mOpenGLWidget->width();
    mProjectionParameters->bottom = mOpenGLWidget->height(); // Flip y-axis
    mProjectionParameters->top = 0;

    mRendererManager->init();
}

void Controller::render()
{
    mProjectionParameters->pixelRatio = mOpenGLWidget->devicePixelRatioF();
    mProjectionParameters->right = mProjectionParameters->left + mOpenGLWidget->width() * mProjectionParameters->zoomRatio;
    mProjectionParameters->bottom = mProjectionParameters->top + mOpenGLWidget->height() * mProjectionParameters->zoomRatio;
    mProjectionParameters->width = mOpenGLWidget->width() * mProjectionParameters->pixelRatio;
    mProjectionParameters->height = mOpenGLWidget->height() * mProjectionParameters->pixelRatio; // Flip y-axis
    mRendererManager->render();
}

void Controller::onAction(Action action, CustomVariant value)
{
    switch (action) {
    case Action::Select: {
        if (mCurveContainer->selectedCurve()) {
            ControlPoint *controlPoint = mCurveContainer->getClosestControlPointOnSelectedCurve(value.toVector2D(),
                                                                                                10 * mProjectionParameters->zoomRatio);
            mCurveContainer->setSelectedControlPoint(controlPoint);

            if (controlPoint) {
                refresh();
                return;
            }
        }

        Curve *selectedCurve = mCurveContainer->selectCurve(value.toVector2D(), 15 * mProjectionParameters->zoomRatio);
        mMousePressedOnCurve = selectedCurve ? true : false;
        mCurveContainer->setSelectedCurve(selectedCurve);
        break;
    }
    case Action::Add: {
        if (mCurveContainer->selectedCurve()) {
            if (mCurveContainer->selectedCurve()->getSize() >= Constants::MAX_CONTROL_POINT_COUNT)
                return;

            ControlPoint *controlPoint = new ControlPoint(value.toVector2D());
            controlPoint->selected = true;
            mCurveContainer->selectedCurve()->addControlPoint(controlPoint);
            mCurveContainer->setSelectedControlPoint(controlPoint);
        } else {
            ControlPoint *controlPoint = new ControlPoint(value.toVector2D());
            controlPoint->selected = true;

            Bezier *curve = new Bezier();
            curve->addControlPoint(controlPoint);
            mCurveContainer->addCurve(curve);
            mCurveContainer->setSelectedCurve(curve);
            mCurveContainer->setSelectedControlPoint(controlPoint);
        }
        break;
    }
    case Action::Move: {
        if (mCurveContainer->selectedCurve())
            mCurveContainer->selectedCurve()->translate(value.toVector2D());
        break;
    }
    case Action::Pan: {
        QVector2D translation = value.toVector2D();
        mProjectionParameters->left += translation.x();
        mProjectionParameters->right += translation.x();
        mProjectionParameters->top += translation.y();
        mProjectionParameters->bottom += translation.y();
        break;
    }
    case Action::RemoveCurve: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->removeCurve(mCurveContainer->selectedCurve());
            mCurveContainer->setSelectedCurve(nullptr);
            mCurveContainer->setSelectedControlPoint(nullptr);
        }
        break;
    }
    case Action::RemoveControlPoint: {
        ControlPoint *selectedControlPoint = mCurveContainer->selectedControlPoint();
        Curve *selectedCurve = mCurveContainer->selectedCurve();
        if (selectedCurve && selectedControlPoint) {
            selectedCurve->removeControlPoint(selectedControlPoint);
            mCurveContainer->setSelectedControlPoint(nullptr);

            if (selectedCurve->getSize() == 0) {
                mCurveContainer->setSelectedCurve(nullptr);
                mCurveContainer->removeCurve(selectedCurve);
            }
        }

        break;
    }
    case Action::UpdateControlPointPosition: {
        if (mCurveContainer->selectedControlPoint()) {
            mCurveContainer->selectedControlPoint()->position = value.toVector2D();
        }
        break;
    }
    case Action::UpdateCurveZIndex: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->selectedCurve()->setZ(value.toInt());
            mCurveContainer->sortCurves();
        }
        break;
    }
    case Action::UpdateContourThickness: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->selectedCurve()->setContourThickness(value.toFloat());
        }
        break;
    }
    case Action::UpdateDiffusionWidth: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->selectedCurve()->setDiffusionWidth(value.toFloat());
        }
        break;
    }
    case Action::ZoomIn: {
        zoom(mProjectionParameters->zoomRatio / mZoomStep, value);
        break;
    }
    case Action::ZoomOut: {
        zoom(mProjectionParameters->zoomRatio * mZoomStep, value);

        break;
    }
    case Action::EnableContourColor: {
        if (mCurveContainer->selectedCurve())
            mCurveContainer->selectedCurve()->setContourColorEnabled(value.toBool());
        break;
    }
    case Action::UpdateContourColor: {
        if (mCurveContainer->selectedCurve())
            mCurveContainer->selectedCurve()->setContourColor(value.toVector4D());
        break;
    }
    }

    refresh();
}

void Controller::onModeChanged(Mode mode)
{
    mMode = mode;

    if (mMode == Mode::Pan) {
        mCurveContainer->setSelectedControlPoint(nullptr);
        mCurveContainer->setSelectedCurve(nullptr);
        refresh();
    }
}

void Controller::onZoomRatioChanged(float newZoomRatio)
{
    zoom(newZoomRatio);
    refresh();
}

void Controller::onWheelMoved(QWheelEvent *event)
{
    int delta = event->angleDelta().y();

    if (delta < 0)
        onAction(Action::ZoomOut, event->position().toPoint());
    if (delta > 0)
        onAction(Action::ZoomIn, event->position().toPoint());
}

void Controller::onMousePressed(QMouseEvent *event)
{
    mMouseRightButtonPressed = event->button() == Qt::RightButton;
    mMouseLeftButtonPressed = event->button() == Qt::LeftButton;
    mMousePosition = event->pos();

    switch (mMode) {
    case Mode::AddColorPoint:
        break;
    case Mode::MoveCurve:
        break;
    case Mode::Pan:
        break;
    case Mode::Select: {
        if (mMouseLeftButtonPressed) {
            QVector2D position = mTransformer->mapFromGuiToOpenGL(mMousePosition);
            onAction(Action::Select, position);
        }
        break;
    }
    case Mode::AddControlPoint: {
        if (mMouseLeftButtonPressed) {
            QVector2D position = mTransformer->mapFromGuiToOpenGL(mMousePosition);
            onAction(Action::Add, position);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void Controller::onMouseReleased(QMouseEvent *event)
{
    mMouseRightButtonPressed = false;
    mMouseLeftButtonPressed = false;
}

void Controller::onMouseMoved(QMouseEvent *event)
{
    if (mMouseRightButtonPressed) {
        QVector2D translation = QVector2D(mMousePosition - event->pos()) * mProjectionParameters->zoomRatio;
        onAction(Action::Pan, translation);
    }

    switch (mMode) {
    case Mode::AddColorPoint:
        break;
    case Mode::Pan: {
        if (mMouseLeftButtonPressed) {
            QVector2D translation = QVector2D(mMousePosition - event->pos()) * mProjectionParameters->zoomRatio;
            onAction(Action::Pan, translation);
        }
        break;
    }
    case Mode::Select: {
        if (mMouseLeftButtonPressed) {
            QVector2D newPosition = mTransformer->mapFromGuiToOpenGL(event->pos());
            onAction(Action::UpdateControlPointPosition, newPosition);
        }
        break;
    }
    case Mode::AddControlPoint: {
        break;
    }
    case Mode::MoveCurve: {
        if (mMouseLeftButtonPressed) {
            if (mCurveContainer->selectedCurve()) {
                QVector2D translation = QVector2D(event->pos() - mMousePosition) * mProjectionParameters->zoomRatio;
                onAction(Action::Move, translation);

            } else {
                QVector2D position = mTransformer->mapFromGuiToOpenGL(mMousePosition);
                onAction(Action::Select, position);
            }
        }
    }
    }

    mMousePosition = event->pos();
}

bool Controller::cursorInsideBoundingBox(QPointF position, QMarginsF margins)
{
    if (mCurveContainer->selectedCurve()) {
        QRectF boundingBox = mCurveContainer->selectedCurve()->getBoundingBox();
        boundingBox = boundingBox.marginsAdded(margins);
        return boundingBox.contains(mTransformer->mapFromGuiToOpenGL(position).toPointF());
    } else
        return false;
}

void Controller::refresh()
{
    mOpenGLWidget->refresh();
    mControlPointWidget->refresh();
    mCurveWidget->refresh();
}

void Controller::zoom(float newZoomRatio, CustomVariant cursorPositionVariant)
{
    if (qFuzzyCompare(newZoomRatio, mProjectionParameters->zoomRatio))
        return;

    QPoint cursorPosition;
    const float width = mOpenGLWidget->width();
    const float height = mOpenGLWidget->width();

    if (cursorPositionVariant.isValid()) {
        cursorPosition = cursorPositionVariant.toPoint();
    } else {
        cursorPosition = QPoint(width / 2, height / 2);
    }

    QVector2D positionBeforeZoom = mTransformer->mapFromGuiToOpenGL(cursorPosition);

    if (newZoomRatio >= 2)
        newZoomRatio = 2;
    if (newZoomRatio <= 1 / 16.0)
        newZoomRatio = 1 / 16.0f;

    mProjectionParameters->zoomRatio = newZoomRatio;

    // Zoom
    mProjectionParameters->right = mProjectionParameters->left + width * mProjectionParameters->zoomRatio;
    mProjectionParameters->bottom = mProjectionParameters->top + height * mProjectionParameters->zoomRatio;

    QVector2D positionAfterZoom = mTransformer->mapFromGuiToOpenGL(cursorPosition);

    float dx = positionBeforeZoom.x() - positionAfterZoom.x();
    float dy = positionBeforeZoom.y() - positionAfterZoom.y();

    mProjectionParameters->left += dx;
    mProjectionParameters->right += dx;
    mProjectionParameters->bottom += dy;
    mProjectionParameters->top += dy;

    emit zoomRatioChanged(mProjectionParameters->zoomRatio);
}

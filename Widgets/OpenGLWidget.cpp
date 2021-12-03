#include "OpenGLWidget.h"

#include <Util.h>

#include <QVector4D>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , mBezierContourRenderer(nullptr)
    , mControlPointRenderer(nullptr)
    , mMode(ModeWidget::Select)
    , mMousePressed(false)
    , mMousePressedOnCurve(false)
    , mMousePosition(0, 0)
    , mZoomRatio(1.0f)
    , mCanvasRectangle(0, 0, 0, 0)
    , mInitialized(false)
{
    setMouseTracking(true);
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();

    if (mBezierContourRenderer)
        delete mBezierContourRenderer;

    if (mControlPointRenderer)
        delete mControlPointRenderer;

    if (mBoundingBoxRenderer)
        delete mBoundingBoxRenderer;

    if (mLineRenderer)
        delete mLineRenderer;

    doneCurrent();
}

QSize OpenGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize OpenGLWidget::sizeHint() const
{
    return QSize(750, 750);
}

void OpenGLWidget::onDirty()
{
    update();
}

void OpenGLWidget::initializeGL()
{
    Bezier *bezier = new Bezier();
    bezier->addControlPoint(new ControlPoint(50.0f, 250.0f));  // First
    bezier->addControlPoint(new ControlPoint(300.0f, 250.0f)); // Last
    mCurveContainer->addCurve(bezier);

    QVector<Curve *> curves = Util::readCurveDataFromXML("Resources/CurveData/zephyr.xml");
    mCurveContainer->addCurves(curves);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
    glEnable(GL_MULTISAMPLE);
    //    glEnable(GL_BLEND);
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mBezierContourRenderer = new BezierContourRenderer;
    mControlPointRenderer = new ControlPointRenderer;
    mBoundingBoxRenderer = new BoundingBoxRenderer;
    mLineRenderer = new LineRenderer;

    mInitialized = true;

    mInitialized &= mBezierContourRenderer->initialize();
    mInitialized &= mControlPointRenderer->initialize();
    mInitialized &= mBoundingBoxRenderer->initialize();
    mInitialized &= mLineRenderer->initialize();

    if (!mInitialized)
        qDebug() << this << "Could not initialize OpenGLWidget";
}
void OpenGLWidget::paintGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    glEnable(GL_DEPTH_TEST);
    //    glEnable(GL_CULL_FACE);

    if (!mInitialized)
        return;

    mBezierContourRenderer->render(mCurveContainer->getCurves());
    mControlPointRenderer->render(mCurveContainer->selectedCurve());
    mBoundingBoxRenderer->render(mCurveContainer->selectedCurve());

    //    {
    //        LineRenderer::RenderParameters params;
    //        params.startingPoint = QVector2D(100, 100);
    //        params.endPoint = QVector2D(500, 500);
    //        params.thickness = 4;
    //        params.color = QVector4D(0,0,0,1);
    //        params.dashLength = 20;
    //        params.gapLength = 5;
    //        params.lineStyle = LineRenderer::Solid;
    //        mLineRenderer->render(params);
    //    }
}

void OpenGLWidget::resizeGL(int width, int height)
{
    if (!mInitialized)
        return;

    mCanvasRectangle.setWidth(width * mZoomRatio);
    mCanvasRectangle.setHeight(height * mZoomRatio);

    updateProjectionMatrix();
    update();
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();

    if (delta < 0)
        zoom(mZoomRatio * 1.125, event->position().toPoint());
    if (delta > 0)
        zoom(mZoomRatio / 1.125, event->position().toPoint());
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    mMousePosition = event->pos();
    mMousePressed = true;

    switch (mMode) {
    case ModeWidget::Pan:
        break;
    case ModeWidget::Select: {
        ControlPoint *controlPoint = mCurveContainer->getClosestControlPointOnSelectedCurve(mapFromGui(mMousePosition), 20 * mZoomRatio);
        mCurveContainer->setSelectedControlPoint(controlPoint);

        if (controlPoint) {
            update();
            return;
        }

        Curve *selectedCurve = mCurveContainer->selectCurve(mapFromGui(mMousePosition), 20 * mZoomRatio);

        mMousePressedOnCurve = selectedCurve ? true : false;
        mCurveContainer->setSelectedCurve(selectedCurve);

        //            if (mSelectedControlPoint)
        //                setSelectedControlPoint(nullptr);
        break;
    }
    case ModeWidget::Add: {
        if (mCurveContainer->selectedCurve()) {
            if (mCurveContainer->selectedCurve()->getSize() >= Constants::MAX_CONTROL_POINT_COUNT)
                return;

            ControlPoint *controlPoint = new ControlPoint(mapFromGui(mMousePosition));
            controlPoint->selected = true;
            mCurveContainer->selectedCurve()->addControlPoint(controlPoint);
            mCurveContainer->setSelectedControlPoint(controlPoint);
        } else {
            ControlPoint *controlPoint = new ControlPoint(mapFromGui(mMousePosition));
            controlPoint->selected = true;

            Bezier *curve = new Bezier();
            curve->addControlPoint(controlPoint);
            mCurveContainer->addCurve(curve);
            mCurveContainer->setSelectedCurve(curve);
            mCurveContainer->setSelectedControlPoint(controlPoint);
        }
        break;
    }
    case ModeWidget::Move: {
        // If there is no selected curve, then we must select one first.
        if (mCurveContainer->selectedCurve() == nullptr) {
            Curve *selectedCurve = mCurveContainer->selectCurve(mapFromGui(mMousePosition), 20 * mZoomRatio);
            mMousePressedOnCurve = selectedCurve ? true : false;

            if (selectedCurve) {
                mCurveContainer->setSelectedCurve(selectedCurve);
                update();
                return;
            }

        } else {
            Curve *selectedCurve = mCurveContainer->selectCurve(mapFromGui(mMousePosition), 20 * mZoomRatio);
            QRectF boundingBox = mCurveContainer->selectedCurve()->getBoundingBox();

            if (boundingBox.contains(mapFromGui(mMousePosition).toPoint())) {
                mMousePressedOnCurve = true;
            } else {
                mCurveContainer->setSelectedCurve(selectedCurve);
                mMousePressedOnCurve = selectedCurve ? true : false;
            }
        }

        break;
    }
    }

    update();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    switch (mMode) {
    case ModeWidget::Pan:
    case ModeWidget::Select:
    case ModeWidget::Add:
    case ModeWidget::Move:
        mMousePressed = false;
        mMousePressedOnCurve = false;
        break;
    }

    update();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePosition = event->pos();

    switch (mMode) {
    case ModeWidget::Pan: {
        if (mMousePressed) {
            QPoint translation = mMousePosition - mousePosition;
            mCanvasRectangle.translate(translation * mZoomRatio);
            updateProjectionMatrix();
        }
        break;
    }
    case ModeWidget::Select: {
        if (mCurveContainer->selectedControlPoint() && mMousePressed) {
            mCurveContainer->selectedControlPoint()->position = mapFromGui(mousePosition);
            emit dirty();
        }
        break;
    }
    case ModeWidget::Add: {
        break;
    }
    case ModeWidget::Move: {
        if (mMousePressedOnCurve) {
            QPointF translation = mousePosition - mMousePosition;

            if (mCurveContainer->selectedCurve())
                mCurveContainer->selectedCurve()->translate(translation * mZoomRatio);
        }

        break;
    }
    }

    mMousePosition = mousePosition;
    update();
}

void OpenGLWidget::update()
{
    QOpenGLWidget::update();
    updateCursor();
}

void OpenGLWidget::updateCursor()
{
    switch (mMode) {
    case ModeWidget::Pan:
        setCursor(mMousePressed ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
        break;
    case ModeWidget::Select:
        setCursor(Qt::ArrowCursor);
        break;
    case ModeWidget::Add:
        setCursor(Qt::CrossCursor);
        break;
    case ModeWidget::Move: {
        if (mCurveContainer->selectedCurve()) {
            if (mCurveContainer->selectedCurve()->getBoundingBox().contains(mapFromGui(mMousePosition).toPointF()))
                setCursor(Qt::SizeAllCursor);
            else
                setCursor(Qt::ArrowCursor);
        } else
            setCursor(Qt::ArrowCursor);
        break;
    }
    }
}

void OpenGLWidget::setCurveContainer(CurveContainer *newCurveContainer)
{
    mCurveContainer = newCurveContainer;
}

void OpenGLWidget::zoom(float zoomRatio, QPoint mousePosition)
{
    if (qFuzzyCompare(zoomRatio, mZoomRatio))
        return;

    QPoint mousePositionOnCanvas = mapFromGui(mousePosition).toPoint();

    if (zoomRatio >= 2)
        zoomRatio = 2;
    if (zoomRatio <= 1 / 16.0)
        zoomRatio = 1 / 16.0f;

    mZoomRatio = zoomRatio;

    mCanvasRectangle.setWidth(width() * mZoomRatio);
    mCanvasRectangle.setHeight(height() * mZoomRatio);

    QPoint newMousePositionOnCanvas = mapFromGui(mousePosition).toPoint();

    mCanvasRectangle.translate(mousePositionOnCanvas - newMousePositionOnCanvas);

    mBoundingBoxRenderer->setZoomRatio(mZoomRatio);
    mControlPointRenderer->setZoomRatio(mZoomRatio);
    mBezierContourRenderer->setZoomRatio(mZoomRatio);

    emit zoomRatioChanged(mZoomRatio);
    updateProjectionMatrix();
    update();
}

void OpenGLWidget::updateProjectionMatrix()
{
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.ortho(mCanvasRectangle.x(),
                 mCanvasRectangle.x() + mCanvasRectangle.width(),
                 mCanvasRectangle.y() + mCanvasRectangle.height(),
                 mCanvasRectangle.y(),
                 -1,
                 1);

    mBezierContourRenderer->setProjectionMatrix(matrix);
    mControlPointRenderer->setProjectionMatrix(matrix);
    mBoundingBoxRenderer->setProjectionMatrix(matrix);
    mLineRenderer->setProjectionMatrix(matrix);
}

void OpenGLWidget::onSelectedControlPointChanged(ControlPoint *selectedControlPoint)
{
    mCurveContainer->setSelectedControlPoint(selectedControlPoint);
    update();
}

void OpenGLWidget::onModeChanged(ModeWidget::Mode mode)
{
    if (mMode == mode)
        return;

    switch (mode) {
    case ModeWidget::Pan:
        mCurveContainer->setSelectedCurve(nullptr);
        mCurveContainer->setSelectedControlPoint(nullptr);
        break;
    case ModeWidget::Select:
        break;
    case ModeWidget::Add:
        break;
    case ModeWidget::Move: {
        mCurveContainer->setSelectedControlPoint(nullptr);
        break;
    }
    }

    mMousePressed = false;
    mMode = mode;

    mControlPointRenderer->setMode(mMode);
    mBoundingBoxRenderer->setMode(mMode);
    mBezierContourRenderer->setMode(mMode);

    update();
    emit dirty();
}

void OpenGLWidget::onAction(Action action)
{
    switch (action) {
    case OpenGLWidget::RemoveCurve: {
        mCurveContainer->removeCurve(mCurveContainer->selectedCurve());
        mCurveContainer->setSelectedCurve(nullptr);
        mCurveContainer->setSelectedControlPoint(nullptr);
        break;
    }
    case OpenGLWidget::RemoveControlPoint: {
        ControlPoint *selectedControlPoint = mCurveContainer->selectedControlPoint();
        Curve *selectedCurve = mCurveContainer->selectedCurve();

        if (selectedCurve && selectedControlPoint) {
            mCurveContainer->setSelectedControlPoint(nullptr);
            selectedCurve->removeControlPoint(selectedControlPoint);
            if (selectedCurve->getSize() == 0) {
                mCurveContainer->setSelectedCurve(nullptr);
                mCurveContainer->removeCurve(selectedCurve);
            }
            update();
        }

        break;
    }
    }
}

void OpenGLWidget::onKeyPressed(Qt::Key key)
{
    if (key == Qt::Key_Delete) {
        if (mCurveContainer->selectedControlPoint())
            onAction(Action::RemoveControlPoint);
        else if (mCurveContainer->selectedCurve())
            onAction(Action::RemoveCurve);
    }
}

void OpenGLWidget::onZoomRatioChanged(float zoomRatio)
{
    zoom(zoomRatio, QPoint(0.5 * width(), 0.5 * height()));
}

void OpenGLWidget::onShowContoursStateChanged(bool state)
{
    mBezierContourRenderer->setShowContours(state);
    update();
}

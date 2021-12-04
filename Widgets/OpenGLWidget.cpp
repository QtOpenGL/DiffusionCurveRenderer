#include "OpenGLWidget.h"

#include <Util.h>

#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QVector4D>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , mBezierContourRenderer(nullptr)
    , mInitialized(false)
    , mMode(ModeWidget::Select)
    , mMousePressed(false)
    , mMousePressedOnCurve(false)
    , mMousePosition(0, 0)
    , mZoomRatio(1.0f)
    , mCanvasRectangle(0, 0, 0, 0)

{
    for (int i = 0; i < 4; ++i)
        mHandles[i].setSize(QSize(10, 10));

    setMouseTracking(true);
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();

    if (mBezierContourRenderer)
        delete mBezierContourRenderer;

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
    mInitialized = mBezierContourRenderer->initialize();

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

    mBezierContourRenderer->render(mCurveContainer->getCurves(), highlightSelectedCurve());

    float pixelRatio = QPaintDevice::devicePixelRatioF();
    QOpenGLPaintDevice device(width() * pixelRatio, height() * pixelRatio);
    QPainter painter(&device);
    painter.setRenderHint(QPainter::Antialiasing, true);

    Curve *selectedCurve = mCurveContainer->selectedCurve();

    switch (mMode) {
    case ModeWidget::Pan:
        break;
    case ModeWidget::Add:
    case ModeWidget::Select: {
        if (selectedCurve) {
            QVector<ControlPoint *> controlPoints = selectedCurve->getControlPoints();

            for (int j = 0; j < controlPoints.size(); ++j) {
                QPointF center = mapToGui(controlPoints[j]->position);

                painter.setPen(QColor(0, 0, 0, 0));

                // Outer disk
                int outerRadius = controlPoints[j]->selected ? 12 : 10;
                painter.setBrush(QColor(122, 120, 120, 128));
                painter.drawEllipse(center, outerRadius, outerRadius);

                // Inner disk
                int innerRadius = 6;
                painter.setBrush(QColor(240, 240, 240));
                painter.drawEllipse(center, innerRadius, innerRadius);
            }

            break;
        }
    }
    case ModeWidget::Move: {
        if (selectedCurve) {
            // Draw bounding box
            QRectF boundingBox = mapToGui(selectedCurve->getBoundingBox());
            QPen dashedPen;
            QVector<qreal> pattern;
            pattern << 4 << 4;
            dashedPen.setDashPattern(pattern);
            dashedPen.setWidth(1);
            dashedPen.setColor(QColor(128, 128, 128));
            painter.setPen(dashedPen);
            painter.drawRect(boundingBox);

            // Draw pivot point
            QPen pen;
            pen.setColor(QColor(128, 128, 128));
            pen.setWidth(1);
            pen.setJoinStyle(Qt::MiterJoin);
            painter.setPen(pen);
            painter.setBrush(QBrush());

            QPointF center = boundingBox.center();
            painter.drawEllipse(center, 6, 6);
            painter.drawLine(center.x() - 10, center.y() + 0.5, center.x() + 10, center.y() + 0.5);
            painter.drawLine(center.x() + 0.5, center.y() - 10, center.x() + 0.5, center.y() + 10);

            // Draw corners
            pen.setColor(QColor(0, 0, 0));
            painter.setPen(pen);
            painter.setBrush(QColor(0, 0, 0, 0));

            for (int i = 0; i < 4; ++i) {
                painter.drawRect(mHandles[i]);
            }
        }
        break;
    }
    }
}

void OpenGLWidget::resizeGL(int width, int height)
{
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
    QPointF mousePosition = event->pos();

    switch (mMode) {
    case ModeWidget::Pan: {
        if (mMousePressed) {
            QPointF translation = mMousePosition - mousePosition;
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

            if (mCurveContainer->selectedCurve()) {
                mCurveContainer->selectedCurve()->translate(translation * mZoomRatio);
            }
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

    Curve *selectedCurve = mCurveContainer->selectedCurve();
    if (selectedCurve) {
        QRectF boundingBox = mapToGui(selectedCurve->getBoundingBox());
        mHandles[0].moveCenter(boundingBox.topLeft());
        mHandles[1].moveCenter(boundingBox.bottomLeft());
        mHandles[2].moveCenter(boundingBox.topRight());
        mHandles[3].moveCenter(boundingBox.bottomRight());
    }

    updateCursor();
}

bool OpenGLWidget::highlightSelectedCurve()
{
    switch (mMode) {
    case ModeWidget::Select:
    case ModeWidget::Add:
    case ModeWidget::Move:
        if (mCurveContainer->selectedCurve())
            return true;
        else
            return false;
    case ModeWidget::Pan:
        return false;
    }
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

QVector2D OpenGLWidget::mapFromGui(QPointF position)
{
    QPointF result = mCanvasRectangle.topLeft() + mZoomRatio * position;
    return QVector2D(result.x(), result.y());
}

QPointF OpenGLWidget::mapToGui(QVector2D position)
{
    float pixelRatio = QPaintDevice::devicePixelRatioF();
    QPointF result = (position.toPoint() - mCanvasRectangle.topLeft()) / mZoomRatio * pixelRatio;
    return result;
}

QRectF OpenGLWidget::mapToGui(const QRectF &rect)
{
    float pixelRatio = QPaintDevice::devicePixelRatioF();
    float w = rect.width() / mZoomRatio * pixelRatio;
    float h = rect.height() / mZoomRatio * pixelRatio;
    QPointF center = rect.center();
    QPointF centerInGui = (center - mCanvasRectangle.topLeft()) / mZoomRatio * pixelRatio;

    return QRectF(centerInGui.x() - 0.5 * w, centerInGui.y() - 0.5 * h, w, h);
}

void OpenGLWidget::setCurveContainer(CurveContainer *newCurveContainer)
{
    mCurveContainer = newCurveContainer;
}

void OpenGLWidget::zoom(float zoomRatio, QPoint mousePosition)
{
    if (qFuzzyCompare(zoomRatio, mZoomRatio))
        return;

    QPointF mousePositionOnCanvas = mapFromGui(mousePosition).toPoint();

    if (zoomRatio >= 2)
        zoomRatio = 2;
    if (zoomRatio <= 1 / 16.0)
        zoomRatio = 1 / 16.0f;

    mZoomRatio = zoomRatio;

    mCanvasRectangle.setWidth(width() * mZoomRatio);
    mCanvasRectangle.setHeight(height() * mZoomRatio);

    QPointF newMousePositionOnCanvas = mapFromGui(mousePosition).toPoint();
    mCanvasRectangle.translate(mousePositionOnCanvas - newMousePositionOnCanvas);
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

    mBezierContourRenderer->setMode(mMode);

    update();
    emit dirty();
}

void OpenGLWidget::onAction(Action action)
{
    switch (action) {
    case Action::ZIndexChanged:
        break;
    case Action::RemoveCurve: {
        mCurveContainer->removeCurve(mCurveContainer->selectedCurve());
        mCurveContainer->setSelectedCurve(nullptr);
        mCurveContainer->setSelectedControlPoint(nullptr);
        break;
    }
    case Action::RemoveControlPoint: {
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

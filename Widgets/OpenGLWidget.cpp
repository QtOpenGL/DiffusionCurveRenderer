#include "OpenGLWidget.h"

#include <Controller.h>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QVector4D>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , mSelectedCurve(nullptr)
    , mSelectedControlPoint(nullptr)
    , mMode(Mode::Select)
    , mInit(false)
    , mMousePressed(false)
    , mMousePosition(0, 0)
{
    for (int i = 0; i < 4; ++i)
        mHandles[i].setSize(QSize(10, 10));

    setMouseTracking(true);
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();

    // TO BE IMPLEMENTED

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

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    mController->initializeOpenGL();
}

void OpenGLWidget::paintGL()
{
    mController->render();

    float pixelRatio = QPaintDevice::devicePixelRatioF();
    QOpenGLPaintDevice device(width() * pixelRatio, height() * pixelRatio);
    QPainter painter(&device);
    painter.setRenderHint(QPainter::Antialiasing, true);

    switch (mMode) {
    case Mode::Pan:
        break;
    case Mode::Add:
    case Mode::Select: {
        if (mSelectedCurve) {
            QVector<const ControlPoint *> controlPoints = mSelectedCurve->getControlPoints();

            for (int j = 0; j < controlPoints.size(); ++j) {
                QPointF center = mTransformer->mapFromOpenGLToGui(controlPoints[j]->position);

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
        }
        break;
    }
    case Mode::Move: {
        if (mSelectedCurve) {
            // Draw bounding box
            QRectF boundingBox = mTransformer->mapFromOpenGLToGui(mSelectedCurve->getBoundingBox());
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

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    emit wheelMoved(event);

    refresh();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    mMousePressed = true;
    emit mousePressed(event);

    refresh();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mMousePressed = false;
    emit mouseReleased(event);

    refresh();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
    mMousePosition = event->pos();
    refresh();
}

void OpenGLWidget::refresh()
{
    if (mSelectedCurve) {
        QRectF boundingBox = mTransformer->mapFromOpenGLToGui(mSelectedCurve->getBoundingBox());
        mHandles[0].moveCenter(boundingBox.topLeft());
        mHandles[1].moveCenter(boundingBox.bottomLeft());
        mHandles[2].moveCenter(boundingBox.topRight());
        mHandles[3].moveCenter(boundingBox.bottomRight());
    }

    switch (mMode) {
    case Mode::Pan:
        setCursor(mMousePressed ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
        break;
    case Mode::Select:
        setCursor(Qt::ArrowCursor);
        break;
    case Mode::Add:
        setCursor(Qt::CrossCursor);
        break;
    case Mode::Move: {
        if (mSelectedCurve)
            setCursor(Qt::SizeAllCursor);
        else
            setCursor(Qt::ArrowCursor);
        break;
    }
    }

    update();
}

void OpenGLWidget::setController(Controller *newController)
{
    mController = newController;
}

void OpenGLWidget::setTransformer(Transformer *newTransformer)
{
    mTransformer = newTransformer;
}

void OpenGLWidget::onSelectedControlPointChanged(const ControlPoint *selectedControlPoint)
{
    mSelectedControlPoint = selectedControlPoint;
    refresh();
}

void OpenGLWidget::onSelectedCurveChanged(const Curve *selectedCurve)
{
    mSelectedCurve = selectedCurve;
    refresh();
}

void OpenGLWidget::onModeChanged(Mode mode)
{
    if (mMode == mode)
        return;

    mMousePressed = false;
    mMode = mode;

    refresh();
}

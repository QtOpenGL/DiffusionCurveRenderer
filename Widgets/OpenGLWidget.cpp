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
    , mMouseRightButtonPressed(false)
    , mMouseLeftButtonPressed(false)
    , mMousePosition(0, 0)
{
    for (int i = 0; i < 4; ++i)
        mHandles[i].setSize(QSize(10, 10));

    mDashedPen.setDashPattern({4, 4});
    mDashedPen.setWidth(1);
    mDashedPen.setColor(QColor(128, 128, 128));
    mDashedPen.setJoinStyle(Qt::MiterJoin);

    mSolidPen.setColor(QColor(128, 128, 128));
    mSolidPen.setWidth(1);
    mSolidPen.setJoinStyle(Qt::MiterJoin);

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
    case Mode::AddColorPoint:
        break;
    case Mode::Pan:
        break;
    case Mode::AddControlPoint:
    case Mode::Select: {
        if (mSelectedCurve) {
            QVector<const ControlPoint *> controlPoints = mSelectedCurve->getControlPoints();

            // Control polygon
            painter.setPen(mDashedPen);
            painter.setBrush(QBrush());

            for (int i = 0; i < controlPoints.size() - 1; ++i) {
                QPointF p0 = mTransformer->mapFromOpenGLToGui(controlPoints[i]->position);
                QPointF p1 = mTransformer->mapFromOpenGLToGui(controlPoints[i + 1]->position);
                painter.drawLine(p0, p1);
            }

            for (int j = 0; j < controlPoints.size(); ++j) {
                QPointF center = mTransformer->mapFromOpenGLToGui(controlPoints[j]->position);

                painter.setPen(QColor(0, 0, 0, 0));

                // Outer disk
                int outerRadius = controlPoints[j]->selected ? 12 : 10;
                outerRadius /= mProjectionParameters->zoomRatio;
                painter.setBrush(QColor(122, 120, 120, 128));
                painter.drawEllipse(center, outerRadius, outerRadius);

                // Inner disk
                int innerRadius = 6;
                innerRadius /= mProjectionParameters->zoomRatio;
                painter.setBrush(QColor(240, 240, 240));
                painter.drawEllipse(center, innerRadius, innerRadius);
            }
        }
        break;
    }
    case Mode::MoveCurve: {
        if (mSelectedCurve) {
            // Draw bounding box
            QRectF boundingBox = mTransformer->mapFromOpenGLToGui(mSelectedCurve->getBoundingBox());
            painter.setPen(mDashedPen);
            painter.drawRect(boundingBox);

            // Draw pivot point
            mSolidPen.setColor(QColor(128, 128, 128));
            painter.setPen(mSolidPen);
            painter.setBrush(QBrush());

            QPointF center = boundingBox.center();
            painter.drawEllipse(center, 6, 6);
            painter.drawLine(center.x() - 10, center.y() + 0.5, center.x() + 10, center.y() + 0.5);
            painter.drawLine(center.x() + 0.5, center.y() - 10, center.x() + 0.5, center.y() + 10);

            // Draw corners
            mSolidPen.setColor(QColor(0, 0, 0));
            painter.setPen(mSolidPen);
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
    mMouseRightButtonPressed = event->button() == Qt::RightButton;
    mMouseLeftButtonPressed = event->button() == Qt::LeftButton;

    emit mousePressed(event);

    refresh();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mMouseRightButtonPressed = false;
    mMouseLeftButtonPressed = false;

    emit mouseReleased(event);

    refresh();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
    mMousePosition = event->pos();
    refresh();
}

void OpenGLWidget::setProjectionParameters(const ProjectionParameters *newProjectionParameters)
{
    mProjectionParameters = newProjectionParameters;
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

    if (mMouseRightButtonPressed) {
        setCursor(Qt::ClosedHandCursor);
        update();
        return;
    }

    switch (mMode) {
    case Mode::AddColorPoint:
        break;
    case Mode::Pan:
        setCursor(mMouseLeftButtonPressed ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
        break;
    case Mode::Select:
        setCursor(Qt::ArrowCursor);
        break;
    case Mode::AddControlPoint:
        setCursor(Qt::CrossCursor);
        break;
    case Mode::MoveCurve: {
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

    mMode = mode;

    refresh();
}

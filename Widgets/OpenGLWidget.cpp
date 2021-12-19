#include "OpenGLWidget.h"

#include <Renderers/RendererManager.h>
#include <Util.h>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QVector4D>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , mRendererManager(nullptr)
    , mSelectedCurve(nullptr)
    , mSelectedControlPoint(nullptr)
    , mMode(Mode::Select)
    , mRenderMode(RenderMode::Diffuse)
    , mInit(false)
    , mMouseLeftButtonPressed(false)
    , mMouseRightButtonPressed(false)
    , mMouseMiddleButtonPressed(false)
    , mMousePosition(0, 0)
{
    for (int i = 0; i < 4; ++i)
        mHandles[i].setSize(QSize(8, 8));

    mDashedPen.setDashPattern({8, 8});
    mDashedPen.setWidthF(0.5f);
    mDashedPen.setJoinStyle(Qt::MiterJoin);

    mSolidPen.setWidthF(0.5f);
    mSolidPen.setJoinStyle(Qt::MiterJoin);

    setMouseTracking(true);
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();

    if (mRendererManager)
        delete mRendererManager;

    mRendererManager = nullptr;

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

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);

    mProjectionParameters->left = 0;
    mProjectionParameters->top = 0;
    mProjectionParameters->zoomRatio = 1.0f;
    mProjectionParameters->pixelRatio = devicePixelRatioF();
    mProjectionParameters->right = mProjectionParameters->left + width() * mProjectionParameters->zoomRatio;
    mProjectionParameters->bottom = mProjectionParameters->top + height() * mProjectionParameters->zoomRatio;
    mProjectionParameters->width = width() * mProjectionParameters->pixelRatio;
    mProjectionParameters->height = height() * mProjectionParameters->pixelRatio; // Flip y-axis

    mRendererManager->init();
}

void OpenGLWidget::paintGL()
{
    mProjectionParameters->pixelRatio = devicePixelRatioF();
    mProjectionParameters->right = mProjectionParameters->left + width() * mProjectionParameters->zoomRatio;
    mProjectionParameters->bottom = mProjectionParameters->top + height() * mProjectionParameters->zoomRatio;
    mProjectionParameters->width = width() * mProjectionParameters->pixelRatio;
    mProjectionParameters->height = height() * mProjectionParameters->pixelRatio; // Flip y-axis

    mRendererManager->render();

    if (mSelectedCurve) {
        QRectF boundingBox = mTransformer->mapFromOpenGLToGui(mSelectedCurve->getBoundingBox());
        mHandles[0].moveCenter(boundingBox.topLeft());
        mHandles[1].moveCenter(boundingBox.bottomLeft());
        mHandles[2].moveCenter(boundingBox.topRight());
        mHandles[3].moveCenter(boundingBox.bottomRight());
    }

    updatePainter();
}

void OpenGLWidget::onDirty(DirtType type)
{
    if (type & DirtType::OpenGL) {
        updateCursor();
        update();
    }
}

void OpenGLWidget::updatePainter()
{
    if (mRenderMode & RenderMode::Diffuse) {
        mDashedPen.setColor(QColor(255, 255, 255));
        mSolidPen.setColor(QColor(255, 255, 255));
    } else {
        mDashedPen.setColor(QColor(0, 0, 0));
        mSolidPen.setColor(QColor(0, 0, 0));
    }

    QOpenGLPaintDevice device(width() * QPaintDevice::devicePixelRatioF(), height() * QPaintDevice::devicePixelRatioF());
    QPainter painter(&device);
    painter.setRenderHint(QPainter::Antialiasing, true);

    switch (mMode) {
    case Mode::Pan:
        break;
    case Mode::AddColorPoint:
    case Mode::AppendControlPoint:
    case Mode::InsertControlPoint:
    case Mode::Select: {
        if (mSelectedCurve) {
            QVector<const ControlPoint *> controlPoints = mSelectedCurve->getControlPoints();

            // Control polygon
            painter.setPen(mDashedPen);
            painter.setBrush(QBrush());
            for (int i = 0; i < controlPoints.size() - 1; ++i) {
                QPointF p0 = mTransformer->mapFromOpenGLToGui(controlPoints[i]->position());
                QPointF p1 = mTransformer->mapFromOpenGLToGui(controlPoints[i + 1]->position());
                painter.drawLine(p0, p1);
            }

            // Control Points
            for (int j = 0; j < controlPoints.size(); ++j) {
                QPointF center = mTransformer->mapFromOpenGLToGui(controlPoints[j]->position());
                painter.setPen(QColor(0, 0, 0, 0));

                // Outer disk
                float outerRadius = controlPoints[j]->selected() ? 16 : 12;
                outerRadius = qMin(outerRadius, outerRadius / mProjectionParameters->zoomRatio);
                painter.setBrush(QColor(128, 128, 128, 128));
                painter.drawEllipse(center, outerRadius, outerRadius);

                // Inner disk
                float innerRadius = 6;
                innerRadius = qMin(innerRadius, innerRadius / mProjectionParameters->zoomRatio);
                painter.setBrush(QColor(255, 255, 255));
                painter.drawEllipse(center, innerRadius, innerRadius);
            }

            // Color points
            QVector<const ColorPoint *> colorPoints = mSelectedCurve->getRightColorPoints();
            colorPoints << mSelectedCurve->getLeftColorPoints();

            for (int i = 0; i < colorPoints.size(); ++i) {
                QPointF center = mTransformer->mapFromOpenGLToGui(colorPoints[i]->getPosition2D());
                painter.setPen(QColor(0, 0, 0, 0));

                // Outer disk
                float outerRadius = colorPoints[i]->selected() ? 16 : 12;
                outerRadius = qMin(outerRadius, outerRadius / mProjectionParameters->zoomRatio);
                painter.setBrush(QColor(0, 0, 0, 128));
                painter.drawEllipse(center, outerRadius, outerRadius);

                // Inner disk
                float innerRadius = 6;
                innerRadius = qMin(innerRadius, innerRadius / mProjectionParameters->zoomRatio);
                painter.setBrush(Util::convertVector4DtoColor(colorPoints[i]->color()));
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
            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.drawRect(boundingBox);
            painter.setRenderHint(QPainter::Antialiasing, true);

            painter.setPen(mSolidPen);

            // Draw pivot point
            QPointF center = boundingBox.center();
            painter.drawEllipse(center, 6, 6);
            painter.drawLine(center.x() - 10, center.y() + 0.5, center.x() + 10, center.y() + 0.5);
            painter.drawLine(center.x() + 0.5, center.y() - 10, center.x() + 0.5, center.y() + 10);

            // Draw corners
            painter.setRenderHint(QPainter::Antialiasing, false);
            for (int i = 0; i < 4; ++i) {
                painter.drawRect(mHandles[i]);
            }
            painter.setRenderHint(QPainter::Antialiasing, true);
        }
        break;
    }
    }
}

void OpenGLWidget::updateCursor()
{
    switch (mMode) {
    case Mode::AppendControlPoint:
        setCursor(Qt::CrossCursor);
        break;
    case Mode::InsertControlPoint:
        setCursor(Qt::CrossCursor);
        break;
    case Mode::AddColorPoint:
        setCursor(Qt::CrossCursor);
        break;
    case Mode::Pan:
        setCursor(mMouseLeftButtonPressed ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
        break;
    case Mode::Select:
        setCursor(Qt::ArrowCursor);
        break;
    case Mode::MoveCurve: {
        if (mSelectedCurve)
            setCursor(Qt::SizeAllCursor);
        else
            setCursor(Qt::ArrowCursor);
        break;
    }
    }
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    emit wheelMoved(event);
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    mMouseRightButtonPressed = event->button() == Qt::RightButton;
    mMouseMiddleButtonPressed = event->button() == Qt::MiddleButton;
    mMouseLeftButtonPressed = event->button() == Qt::LeftButton;

    emit mousePressed(event);
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mMouseRightButtonPressed = false;
    mMouseMiddleButtonPressed = false;
    mMouseLeftButtonPressed = false;

    emit mouseReleased(event);
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
    mMousePosition = event->pos();
}

void OpenGLWidget::setRendererManager(RendererManager *newRendererManager)
{
    mRendererManager = newRendererManager;
}

float OpenGLWidget::getWidth()
{
    return width();
}

float OpenGLWidget::getHeight()
{
    return height();
}

void OpenGLWidget::makeCurrent()
{
    QOpenGLWidget::makeCurrent();
}

void OpenGLWidget::doneCurrent()
{
    QOpenGLWidget::doneCurrent();
}

void OpenGLWidget::setProjectionParameters(ProjectionParameters *newProjectionParameters)
{
    mProjectionParameters = newProjectionParameters;
}

void OpenGLWidget::setTransformer(Transformer *newTransformer)
{
    mTransformer = newTransformer;
}

void OpenGLWidget::onSelectedControlPointChanged(const ControlPoint *selectedControlPoint)
{
    mSelectedControlPoint = selectedControlPoint;
}

void OpenGLWidget::onSelectedCurveChanged(const Curve *selectedCurve)
{
    mSelectedCurve = selectedCurve;
}

void OpenGLWidget::onSelectedColorPointChanged(const ColorPoint *selectedColorPoint)
{
    mSelectedColorPoint = selectedColorPoint;
}

void OpenGLWidget::onModeChanged(Mode mode)
{
    mMode = mode;
}

void OpenGLWidget::onRenderModeChanged(RenderMode renderMode)
{
    mRenderMode = renderMode;
}

#include "OpenGLWidget.h"

#include <Renderers/RendererManager.h>
#include <Util.h>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QVector4D>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , mSelectedCurve(nullptr)
    , mSelectedControlPoint(nullptr)
    , mMode(Mode::Select)
    , mRenderMode(RenderMode::Contours)
    , mInit(false)
    , mMouseRightButtonPressed(false)
    , mMouseLeftButtonPressed(false)
    , mMousePosition(0, 0)
    , mUpdatePainter(false)
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

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mRenderMode & RenderMode::Diffuse) {
        mRendererManager->diffuse();
    }

    glViewport(0, 0, mProjectionParameters->width, mProjectionParameters->height);

    if (mRenderMode & RenderMode::Contours) {
        mRendererManager->contours();
    }

    if (mSelectedCurve) {
        QRectF boundingBox = mTransformer->mapFromOpenGLToGui(mSelectedCurve->getBoundingBox());
        mHandles[0].moveCenter(boundingBox.topLeft());
        mHandles[1].moveCenter(boundingBox.bottomLeft());
        mHandles[2].moveCenter(boundingBox.topRight());
        mHandles[3].moveCenter(boundingBox.bottomRight());
    }

    // None?
    updatePainter();
}

void OpenGLWidget::onDirty(DirtType type)
{
    if (type & DirtType::OpenGL) {
        update();
        updateCursor();
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
                float outerRadius = controlPoints[j]->selected() ? 14 : 10;
                outerRadius /= mProjectionParameters->zoomRatio;
                painter.setBrush(QColor(128, 128, 128, 128));
                painter.drawEllipse(center, outerRadius, outerRadius);

                // Inner disk
                float innerRadius = 6;
                innerRadius /= mProjectionParameters->zoomRatio;
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
                float outerRadius = colorPoints[i]->selected() ? 12 : 8;
                outerRadius /= mProjectionParameters->zoomRatio;
                painter.setBrush(QColor(128, 128, 128, 128));
                painter.drawEllipse(center, outerRadius, outerRadius);

                // Inner disk
                float innerRadius = 4;
                innerRadius /= mProjectionParameters->zoomRatio;
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
    if (mMouseRightButtonPressed) {
        setCursor(Qt::ClosedHandCursor);
        update();
        return;
    }

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
    mMouseLeftButtonPressed = event->button() == Qt::LeftButton;

    emit mousePressed(event);
    updateCursor();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mMouseRightButtonPressed = false;
    mMouseLeftButtonPressed = false;

    emit mouseReleased(event);
    updateCursor();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
    mMousePosition = event->pos();
    updateCursor();
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
    update();
}

void OpenGLWidget::onSelectedCurveChanged(const Curve *selectedCurve)
{
    mSelectedCurve = selectedCurve;
    update();
}

void OpenGLWidget::onSelectedColorPointChanged(const ColorPoint *selectedColorPoint)
{
    mSelectedColorPoint = selectedColorPoint;
    update();
}

void OpenGLWidget::onModeChanged(Mode mode)
{
    mMode = mode;
    update();
}

void OpenGLWidget::onRenderModeChanged(RenderMode renderMode)
{
    mRenderMode = renderMode;
    update();
}

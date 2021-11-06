#include "GLWidget.h"

#include <QVector4D>
#include <Util.h>

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent),
    mBezierContourRenderer(nullptr),
    mControlPointRenderer(nullptr),
    mSelectedCurve(nullptr),
    mSelectedControlPoint(nullptr),
    mMode(ModeWidget::Select),
    mMousePressed(false),
    mMousePressedOnCurve(false),
    mMousePosition(0,0),
    mZoomRatio(1.0f),
    mCanvasRectangle(0,0,0,0),
    mInitialized(false)
{
    //    {
    //        Bezier* bezier = new Bezier;
    //        bezier->addControlPoint(new ControlPoint(250, 500));// First
    //        bezier->addControlPoint(new ControlPoint(750, 500)); // Last
    //        bezier->addControlPoint(new ControlPoint(150, 250));
    //        bezier->addControlPoint(new ControlPoint(200, 50));
    //        bezier->setColor(QVector4D(0.4f, 0.32f, 0.82f, 1.0f));
    //        mCurves << bezier;
    //    }

    //    {
    //        Bezier* bezier = new Bezier;
    //        bezier->addControlPoint(new ControlPoint(50, 550));// First
    //        bezier->addControlPoint(new ControlPoint(300, 550)); // Last
    //        bezier->addControlPoint(new ControlPoint(250, 650));
    //        bezier->setColor(QVector4D(0.74f, 0.3f, 0.3f, 1.0f));
    //        mCurves << bezier;
    //    }

    {
        Bezier* bezier = new Bezier();
        bezier->addControlPoint(new ControlPoint(50.0f, 250.0f));// First
        bezier->addControlPoint(new ControlPoint(300.0f, 250.0f)); // Last
        mCurves << bezier;
    }

    mCurves = Util::readCurveDataFromXML("Data/zephyr.xml");

    setMouseTracking(true);
}

GLWidget::~GLWidget()
{
    for(auto &curve : mCurves)
    {
        if(curve)
            delete curve;

        curve = nullptr;
    }
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(750, 750);
}

void GLWidget::onDirty()
{
    update();
}

#include <QApplication>

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 0);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mBezierContourRenderer = new BezierContourRenderer(this);
    mControlPointRenderer = new ControlPointRenderer(this);
    mBoundingBoxRenderer = new BoundingBoxRenderer(this);
    mLineRenderer = new LineRenderer(this);

    mInitialized = true;

    mInitialized &= mBezierContourRenderer->initialize();
    mInitialized &= mControlPointRenderer->initialize();
    mInitialized &= mBoundingBoxRenderer->initialize();
    mInitialized &= mLineRenderer->initialize();

    if(!mInitialized)
        qDebug() << this << "Could not initialize GLWidget.";

}

void GLWidget::paintGL()
{
    if(!mInitialized)
        return;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    glEnable(GL_DEPTH_TEST);
    //    glEnable(GL_CULL_FACE);

    mBezierContourRenderer->render(mCurves);
    mControlPointRenderer->render(mSelectedCurve);
    mBoundingBoxRenderer->render(mSelectedCurve);

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

void GLWidget::resizeGL(int width, int height)
{
    if(!mInitialized)
        return;

    mCanvasRectangle.setWidth(width * mZoomRatio);
    mCanvasRectangle.setHeight(height * mZoomRatio);

    updateProjectionMatrix();
    update();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();

    if(delta < 0)
        zoom(mZoomRatio * 1.125, event->position());
    if(delta > 0)
        zoom(mZoomRatio / 1.125, event->position());

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    mMousePosition = event->pos();
    mMousePressed = true;

    switch (mMode) {
    case ModeWidget::Pan:
        break;
    case ModeWidget::Select:
    {
        if(mSelectedCurve)
        {
            ControlPoint* controlPoint = mSelectedCurve->getClosestControlPoint(g2c(mMousePosition));

            if(controlPoint)
            {
                if(controlPoint->position.distanceToPoint(QVector2D(g2c(mMousePosition))) > 20 * mZoomRatio)
                    controlPoint = nullptr;
            }



            setSelectedControlPoint(controlPoint);
            if(controlPoint)
            {
                update();
                return;
            }
            else
                setSelectedCurve(nullptr);
        }

        Curve* selectedCurve = selectCurve(mMousePosition);

        if(selectedCurve)
        {
            mMousePressedOnCurve = true;
            setSelectedCurve(selectedCurve);
            if(mSelectedControlPoint)
                setSelectedControlPoint(nullptr);
        }
        else
        {
            mMousePressedOnCurve = false;
        }

        break;
    }
    case ModeWidget::Add:
    {
        if(mSelectedCurve)
        {
            if(mSelectedCurve->getSize() >= Constants::MAX_CONTROL_POINT_COUNT)
                return;

            ControlPoint* controlPoint = new ControlPoint(g2c(mMousePosition));
            controlPoint->selected = true;
            mSelectedCurve->addControlPoint(controlPoint);
            setSelectedControlPoint(controlPoint);
        }
        else
        {
            ControlPoint* controlPoint = new ControlPoint(g2c(mMousePosition));
            controlPoint->selected = true;

            Bezier* curve = new Bezier();
            curve->addControlPoint(controlPoint);

            mCurves << curve;

            setSelectedCurve(curve);
            setSelectedControlPoint(controlPoint);
        }
        break;
    }
    case ModeWidget::Move:
    {
        // If there is no selected curve, then we must select one first.
        if(mSelectedCurve == nullptr)
        {
            Curve* selectedCurve = selectCurve(mMousePosition);
            if(selectedCurve)
            {
                mMousePressedOnCurve = true;
                setSelectedCurve(selectedCurve);
                update();
                return;
            }
            else
            {
                mMousePressedOnCurve = false;
            }
        }
        else
        {
            Curve* selectedCurve = selectCurve(mMousePosition);
            QRectF boundingBox = mSelectedCurve->getBoundingBox();

            if(boundingBox.contains(g2c(mMousePosition)))
            {
                mMousePressedOnCurve = true;
            }
            else
            {
                setSelectedCurve(selectedCurve);
                mMousePressedOnCurve = selectedCurve ? true : false;
            }

        }

        break;
    }
    }

    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
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

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPointF mousePosition = event->pos();

    switch (mMode) {
    case ModeWidget::Pan:
    {
        if(mMousePressed)
        {
            QPointF translation =  mMousePosition - mousePosition;
            mCanvasRectangle.translate(translation * mZoomRatio);
            updateProjectionMatrix();
        }
        break;
    }
    case ModeWidget::Select:
    {
        if(mSelectedControlPoint && mMousePressed)
        {
            mSelectedControlPoint->position = QVector2D(g2c(mousePosition));
            emit dirty();
        }
        break;
    }
    case ModeWidget::Add:
    {
        break;
    }
    case ModeWidget::Move:
    {
        if(mMousePressedOnCurve)
        {
            QPointF translation =  mousePosition - mMousePosition;

            if(mSelectedCurve)
                mSelectedCurve->translate(translation * mZoomRatio);
        }

        break;
    }
    }

    mMousePosition = mousePosition;
    update();
}

void GLWidget::update()
{
    QOpenGLWidget::update();
    updateCursor();
}

void GLWidget::deselectAllCurves()
{
    for(int i = 0; i < mCurves.size(); ++i)
        mCurves[i]->setSelected(false);

    mSelectedCurve = nullptr;
}

void GLWidget::setSelectedCurve(Curve *selectedCurve)
{
    if(mSelectedCurve == selectedCurve)
        return;

    if(mSelectedCurve)
        mSelectedCurve->setSelected(false);

    if(selectedCurve)
        selectedCurve->setSelected(true);

    mSelectedCurve = selectedCurve;
    emit selectedCurveChanged(selectedCurve);
}

void GLWidget::setSelectedControlPoint(ControlPoint *selectedControlPoint)
{
    if(mSelectedControlPoint == selectedControlPoint)
        return;

    if(mSelectedControlPoint)
        mSelectedControlPoint->selected = false;

    if(selectedControlPoint)
        selectedControlPoint->selected = true;

    mSelectedControlPoint = selectedControlPoint;
    emit selectedControlPointChanged(selectedControlPoint);
}

void GLWidget::updateCursor()
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
    case ModeWidget::Move:
    {
        if(mSelectedCurve)
        {
            if(mSelectedCurve->getBoundingBox().contains(g2c(mMousePosition)))
                setCursor(Qt::SizeAllCursor);
            else
                setCursor(Qt::ArrowCursor);
        }
        else
            setCursor(Qt::ArrowCursor);
        break;
    }

    }
    
}

ControlPoint *GLWidget::getClosestControlPoint(QPointF mousePosition)
{
    if(!mSelectedCurve)
        return nullptr;

    ControlPoint* controlPoint = mSelectedCurve->getClosestControlPoint(g2c(mousePosition));

    if(controlPoint)
    {
        if(controlPoint->position.distanceToPoint(QVector2D(g2c(mousePosition))) > 20 * mZoomRatio)
            controlPoint = nullptr;
    }

    return controlPoint;
}

void GLWidget::removeCurve(int index)
{
    if(0 <= index && index < mCurves.size())
    {
        Curve* curve = mCurves[index];
        mCurves.removeAt(index);
        delete curve;
    }

}

void GLWidget::removeCurve(Curve *curve)
{
    for(int i = 0; i < mCurves.size(); ++i)
    {
        if(mCurves[i] == curve)
        {
            removeCurve(i);
            return;
        }
    }
}

Curve *GLWidget::selectCurve(QPointF mousePosition, float radius)
{
    float minDistance = std::numeric_limits<float>::infinity();

    Curve* selectedCurve = nullptr;

    for(int i = 0; i < mCurves.size(); ++i)
    {
        float distance = mCurves[i]->distanceToPoint(g2c(mousePosition));
        if(distance < minDistance)
        {
            minDistance = distance;
            selectedCurve = mCurves[i];
        }
    }

    if(minDistance < radius)
        return selectedCurve;

    return nullptr;
}

void GLWidget::zoom(float zoomRatio, QPointF mousePosition)
{
    if(qFuzzyCompare(zoomRatio, mZoomRatio))
        return;

    QPointF mousePositionOnCanvas = g2c(mousePosition);

    if(zoomRatio >= 2)
        zoomRatio = 2;
    if(zoomRatio <= 1 / 16.0)
        zoomRatio = 1 / 16.0f;

    mZoomRatio = zoomRatio;

    mCanvasRectangle.setWidth(width() * mZoomRatio);
    mCanvasRectangle.setHeight(height() * mZoomRatio);

    QPointF newMousePositionOnCanvas = g2c(mousePosition);

    mCanvasRectangle.translate(mousePositionOnCanvas - newMousePositionOnCanvas);

    mBoundingBoxRenderer->setZoomRatio(mZoomRatio);
    mControlPointRenderer->setZoomRatio(mZoomRatio);
    mBezierContourRenderer->setZoomRatio(mZoomRatio);

    emit zoomRatioChanged(mZoomRatio);
    updateProjectionMatrix();
    update();
}

void GLWidget::updateProjectionMatrix()
{
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.ortho(mCanvasRectangle.x(), mCanvasRectangle.x() + mCanvasRectangle.width(), mCanvasRectangle.y() + mCanvasRectangle.height(),mCanvasRectangle.y() , -1, 1);

    mBezierContourRenderer->setProjectionMatrix(matrix);
    mControlPointRenderer->setProjectionMatrix(matrix);
    mBoundingBoxRenderer->setProjectionMatrix(matrix);
    mLineRenderer->setProjectionMatrix(matrix);
}

void GLWidget::onSelectedControlPointChanged(ControlPoint *selectedControlPoint)
{
    setSelectedControlPoint(selectedControlPoint);
    update();
}

void GLWidget::onModeChanged(ModeWidget::Mode mode)
{
    if(mMode == mode)
        return;

    switch (mode) {
    case ModeWidget::Pan:
        setSelectedCurve(nullptr);
        setSelectedControlPoint(nullptr);
        break;
    case ModeWidget::Select:
        break;
    case ModeWidget::Add:
        break;
    case ModeWidget::Move:
    {
        setSelectedControlPoint(nullptr);
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

void GLWidget::onAction(Action action)
{
    switch (action) {
    case GLWidget::RemoveCurve:
    {
        if(mSelectedCurve)
        {
            for(int i = 0; i < mCurves.size(); ++i)
            {
                Curve* curve = mCurves[i];

                if(mSelectedCurve == curve)
                {
                    setSelectedCurve(nullptr);
                    setSelectedControlPoint(nullptr);
                    removeCurve(i);
                    update();
                    return;
                }
            }
        }
        break;
    }
    case GLWidget::RemoveControlPoint:
    {
        ControlPoint* selectedControlPoint = mSelectedControlPoint;
        Curve* selectedCurve = mSelectedCurve;

        if(selectedCurve && selectedControlPoint)
        {
            setSelectedControlPoint(nullptr);
            mSelectedCurve->removeControlPoint(selectedControlPoint);
            if(mSelectedCurve->getSize() == 0)
            {
                setSelectedCurve(nullptr);
                removeCurve(selectedCurve);
            }
            update();
        }

        break;
    }
    }
}

void GLWidget::onKeyPressed(int key)
{
    if(key == 16777223)
    {
        if(mSelectedControlPoint)
            onAction(Action::RemoveControlPoint);
        else if(mSelectedCurve)
            onAction(Action::RemoveCurve);
    }
}

void GLWidget::onZoomRatioChanged(float zoomRatio)
{
    zoom(zoomRatio, QPointF(0.5 * width(), 0.5 * height()));
}

void GLWidget::onShowContoursStateChanged(bool state)
{
    mBezierContourRenderer->setShowContours(state);
    update();
}

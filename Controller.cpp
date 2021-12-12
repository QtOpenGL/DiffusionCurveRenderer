#include "Controller.h"
#include "Util.h"

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

    mProjectionParameters->left = 0;
    mProjectionParameters->top = 0;
    mProjectionParameters->zoomRatio = 0;
    mProjectionParameters->pixelRatio = 0;
    mProjectionParameters->right = 0;
    mProjectionParameters->bottom = 0;
    mProjectionParameters->width = 0;
    mProjectionParameters->height = 0;

    mTransformer->setProjectionParameters(mProjectionParameters);

    mOpenGLWidget->setTransformer(mTransformer);
    mOpenGLWidget->setProjectionParameters(mProjectionParameters);
    mOpenGLWidget->setRendererManager(mRendererManager);

    mRendererManager->setCurveContainer(mCurveContainer);
    mRendererManager->setProjectionParameters(mProjectionParameters);

    connect(mCurveContainer, &CurveContainer::selectedCurveChanged, mCurveWidget, &CurveWidget::onSelectedCurveChanged);
    connect(mCurveContainer, &CurveContainer::selectedCurveChanged, mControlPointWidget, &ControlPointWidget::onSelectedCurveChanged);
    connect(mCurveContainer, &CurveContainer::selectedCurveChanged, mOpenGLWidget, &OpenGLWidget::onSelectedCurveChanged);
    connect(mCurveContainer, &CurveContainer::selectedCurveChanged, mColorPointWidget, &ColorPointWidget::onSelectedCurveChanged);
    connect(mCurveContainer, &CurveContainer::selectedCurveChanged, mModeWidget, &ModeWidget::onSelectedCurveChanged);
    connect(mCurveContainer, &CurveContainer::selectedControlPointChanged, mOpenGLWidget, &OpenGLWidget::onSelectedControlPointChanged);
    connect(mCurveContainer, &CurveContainer::selectedControlPointChanged, mControlPointWidget, &ControlPointWidget::onSelectedControlPointChanged);
    connect(mCurveContainer, &CurveContainer::selectedColorPointChanged, mColorPointWidget, &ColorPointWidget::onSelectedColorPointChanged);
    connect(mCurveContainer, &CurveContainer::selectedColorPointChanged, mOpenGLWidget, &OpenGLWidget::onSelectedColorPointChanged);

    connect(mModeWidget, &ModeWidget::modeChanged, mOpenGLWidget, &OpenGLWidget::onModeChanged);
    connect(mModeWidget, &ModeWidget::modeChanged, this, &Controller::onModeChanged);

    connect(mOpenGLWidget, &OpenGLWidget::wheelMoved, this, &Controller::onWheelMoved);
    connect(mOpenGLWidget, &OpenGLWidget::mousePressed, this, &Controller::onMousePressed);
    connect(mOpenGLWidget, &OpenGLWidget::mouseReleased, this, &Controller::onMouseReleased);
    connect(mOpenGLWidget, &OpenGLWidget::mouseMoved, this, &Controller::onMouseMoved);

    connect(mCurveWidget, &CurveWidget::action, this, &Controller::onAction);
    connect(mControlPointWidget, &ControlPointWidget::action, this, &Controller::onAction);
    connect(mColorPointWidget, &ColorPointWidget::action, this, &Controller::onAction);
    connect(mZoomWidget, &ZoomWidget::zoomRatioChanged, this, &Controller::onZoomRatioChanged);
    connect(mRenderSettingsWidget, &RenderSettingsWidget::action, this, &Controller::onAction);
    connect(mRenderSettingsWidget, &RenderSettingsWidget::renderModeChanged, mOpenGLWidget, &OpenGLWidget::onRenderModeChanged);

    connect(this, &Controller::zoomRatioChanged, mZoomWidget, &ZoomWidget::onZoomRatioChanged);
    connect(this, &Controller::dirty, mOpenGLWidget, &OpenGLWidget::onDirty);
    connect(this, &Controller::dirty, mModeWidget, &ModeWidget::onDirty);
    connect(this, &Controller::dirty, mCurveWidget, &CurveWidget::onDirty);
    connect(this, &Controller::dirty, mControlPointWidget, &ControlPointWidget::onDirty);
    connect(this, &Controller::dirty, mColorPointWidget, &ColorPointWidget::onDirty);

    //    Bezier *bezier = new Bezier;
    //    bezier->addControlPoint(new ControlPoint(100.0f, 512.0f)); // First
    //    bezier->addControlPoint(new ControlPoint(600.0f, 800.0f)); // Last
    //    bezier->addColorPoint(new ColorPoint(bezier, 0.0f, QVector4D(1, 0, 0, 1), ColorPoint::Left));
    //    bezier->addColorPoint(new ColorPoint(bezier, 0.5f, QVector4D(0, 0, 0, 1), ColorPoint::Left));
    //    bezier->addColorPoint(new ColorPoint(bezier, 1.0f, QVector4D(0, 0, 1, 1), ColorPoint::Left));
    //    bezier->addColorPoint(new ColorPoint(bezier, 0.0f, QVector4D(0, 1, 0, 1), ColorPoint::Right));
    //    bezier->addColorPoint(new ColorPoint(bezier, 0.25f, QVector4D(1, 1, 1, 1), ColorPoint::Right));
    //    bezier->addColorPoint(new ColorPoint(bezier, 0.5f, QVector4D(1, 1, 0, 1), ColorPoint::Right));
    //    bezier->addColorPoint(new ColorPoint(bezier, 0.75f, QVector4D(0, 0, 0, 1), ColorPoint::Right));
    //    bezier->addColorPoint(new ColorPoint(bezier, 1.0f, QVector4D(0, 1, 1, 1), ColorPoint::Right));
    //    mCurveContainer->addCurve(bezier);

    QVector<Curve *> curves = Util::readCurveDataFromXML("Resources/CurveData/poivron.xml");
    mCurveContainer->addCurves(curves);
}

CentralWidget *Controller::centralWidget() const
{
    return mCentralWidget;
}

void Controller::onAction(Action action, CustomVariant value)
{
    switch (action) {
    case Action::UpdateSmoothIterations:
        mRendererManager->setSmoothIterations(value.toInt());

        emit dirty(DirtType::OpenGL + DirtType::GUI);
        break;
    case Action::UpdateQuality:
        mRendererManager->setQuality(value.toInt());

        emit dirty(DirtType::OpenGL + DirtType::GUI);
        break;
    case Action::AddColorPoint: {
        if (mCurveContainer->selectedCurve() && mCurveContainer->selectedCurve()->getSize() >= 2) {
            QVector2D nearbyPosition = value.toVector2D();
            float parameter = mCurveContainer->selectedCurve()->parameterAt(nearbyPosition);
            QVector3D positionOnCurve = mCurveContainer->selectedCurve()->valueAt(parameter).toVector3D();
            QVector3D tangent = mCurveContainer->selectedCurve()->tangentAt(parameter).toVector3D();
            QVector3D direction = (nearbyPosition.toVector3D() - positionOnCurve).normalized();
            QVector3D cross = QVector3D::crossProduct(tangent, direction);

            ColorPoint::Type type = cross.z() > 0 ? ColorPoint::Left : ColorPoint::Right;

            ColorPoint *colorPoint = new ColorPoint(mCurveContainer->selectedCurve());
            colorPoint->setPosition(parameter);
            colorPoint->setType(type);
            colorPoint->setColor(QVector4D(1, 1, 1, 1));

            if (mCurveContainer->selectedCurve()->addColorPoint(colorPoint)) {
                mCurveContainer->setSelectedColorPoint(colorPoint);
                mCurveContainer->setSelectedControlPoint(nullptr);
            } else {
                colorPoint->deleteLater();
            }

            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::RemoveColorPoint: {
        ColorPoint *selectedColorPoint = mCurveContainer->selectedColorPoint();

        Curve *selectedCurve = mCurveContainer->selectedCurve();
        if (selectedCurve && selectedColorPoint) {
            selectedCurve->removeColorPoint(selectedColorPoint);
            mCurveContainer->setSelectedColorPoint(nullptr);

            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateColorPointColor: {
        ColorPoint *selectedColorPoint = mCurveContainer->selectedColorPoint();
        if (selectedColorPoint) {
            selectedColorPoint->setColor(value.toVector4D());

            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateColorPointPosition: {
        ColorPoint *selectedColorPoint = mCurveContainer->selectedColorPoint();
        if (selectedColorPoint) {
            selectedColorPoint->setPosition(value.toFloat());

            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::Select: {
        if (mCurveContainer->selectedCurve()) {
            QVector2D nearbyPoint = value.toVector2D();
            ControlPoint *controlPoint = mCurveContainer->getClosestControlPointOnSelectedCurve(nearbyPoint, 10);
            ColorPoint *colorPoint = mCurveContainer->getClosestColorPointOnSelectedCurve(nearbyPoint, 20);
            bool flag = false;

            if (controlPoint && colorPoint) {
                float distanceToControlPoint = nearbyPoint.distanceToPoint(controlPoint->position());
                float distanceToColorPoint = nearbyPoint.distanceToPoint(colorPoint->getPosition2D());

                mCurveContainer->setSelectedControlPoint(distanceToColorPoint > distanceToControlPoint ? controlPoint : nullptr);
                mCurveContainer->setSelectedColorPoint(distanceToColorPoint < distanceToControlPoint ? colorPoint : nullptr);
                flag = true;

            } else if (controlPoint) {
                mCurveContainer->setSelectedControlPoint(controlPoint);
                mCurveContainer->setSelectedColorPoint(nullptr);
                flag = true;
            } else if (colorPoint) {
                mCurveContainer->setSelectedControlPoint(nullptr);
                mCurveContainer->setSelectedColorPoint(colorPoint);
                flag = true;
            } else {
                mCurveContainer->setSelectedControlPoint(nullptr);
                mCurveContainer->setSelectedColorPoint(nullptr);
            }

            if (flag) {
                emit dirty(DirtType::QPainter + DirtType::GUI);
                return;
            }
        }

        Curve *selectedCurve = mCurveContainer->selectCurve(value.toVector2D(), 15 * mProjectionParameters->zoomRatio);
        mMousePressedOnCurve = selectedCurve ? true : false;
        mCurveContainer->setSelectedCurve(selectedCurve);

        emit dirty(DirtType::QPainter + DirtType::GUI);
        break;
    }
    case Action::AppendControlPoint:
    case Action::InsertControlPoint: {
        if (mCurveContainer->selectedCurve()) {
            if (mCurveContainer->selectedCurve()->getSize() >= Constants::MAX_CONTROL_POINT_COUNT)
                return;

            ControlPoint *controlPoint = new ControlPoint(value.toVector2D());
            controlPoint->setSelected(true);
            mCurveContainer->selectedCurve()->addControlPoint(controlPoint, action == Action::AppendControlPoint);
            mCurveContainer->setSelectedControlPoint(controlPoint);
            mCurveContainer->setSelectedColorPoint(nullptr);
        } else {
            ControlPoint *controlPoint = new ControlPoint(value.toVector2D());
            controlPoint->setSelected(true);

            Bezier *curve = new Bezier();
            curve->addControlPoint(controlPoint);
            mCurveContainer->addCurve(curve);
            mCurveContainer->setSelectedCurve(curve);
            mCurveContainer->setSelectedControlPoint(controlPoint);
            mCurveContainer->setSelectedColorPoint(nullptr);
        }

        emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        break;
    }
    case Action::Move: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->selectedCurve()->translate(value.toVector2D());

            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::Pan: {
        QVector2D translation = value.toVector2D();
        mProjectionParameters->left += translation.x();
        mProjectionParameters->right += translation.x();
        mProjectionParameters->top += translation.y();
        mProjectionParameters->bottom += translation.y();

        emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        break;
    }
    case Action::RemoveCurve: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->removeCurve(mCurveContainer->selectedCurve());
            mCurveContainer->setSelectedControlPoint(nullptr);
            mCurveContainer->setSelectedCurve(nullptr);
        }

        emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
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

        emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        break;
    }
    case Action::UpdateControlPointPosition: {
        if (mCurveContainer->selectedControlPoint()) {
            mCurveContainer->selectedControlPoint()->setPosition(value.toVector2D());
            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateControlPointXPosition: {
        if (mCurveContainer->selectedControlPoint()) {
            QVector2D position = mCurveContainer->selectedControlPoint()->position();
            position.setX(value.toFloat());
            mCurveContainer->selectedControlPoint()->setPosition(position);

            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateControlPointYPosition: {
        if (mCurveContainer->selectedControlPoint()) {
            QVector2D position = mCurveContainer->selectedControlPoint()->position();
            position.setY(value.toFloat());
            mCurveContainer->selectedControlPoint()->setPosition(position);

            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateCurveZIndex: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->selectedCurve()->setZ(value.toInt());
            mCurveContainer->sortCurves();

            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateContourThickness: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->selectedCurve()->setContourThickness(value.toFloat());

            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateDiffusionWidth: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->selectedCurve()->setDiffusionWidth(value.toFloat());

            emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        }
        break;
    }
    case Action::ZoomIn: {
        zoom(mProjectionParameters->zoomRatio / mZoomStep, value);

        emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        break;
    }
    case Action::ZoomOut: {
        zoom(mProjectionParameters->zoomRatio * mZoomStep, value);

        emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        break;
    }
    case Action::EnableContourColor: {
        if (mCurveContainer->selectedCurve())
            mCurveContainer->selectedCurve()->setContourColorEnabled(value.toBool());

        emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        break;
    }
    case Action::UpdateContourColor: {
        if (mCurveContainer->selectedCurve())
            mCurveContainer->selectedCurve()->setContourColor(value.toVector4D());
        emit dirty(DirtType::OpenGL + DirtType::QPainter + DirtType::GUI);
        break;
    }
    }
}

void Controller::onModeChanged(Mode mode)
{
    mMode = mode;

    if (mMode == Mode::Pan) {
        mCurveContainer->setSelectedColorPoint(nullptr);
        mCurveContainer->setSelectedControlPoint(nullptr);
        mCurveContainer->setSelectedCurve(nullptr);
    }
}

void Controller::onZoomRatioChanged(float newZoomRatio)
{
    zoom(newZoomRatio);

    emit dirty(DirtType::OpenGL + DirtType::QPainter);
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
    case Mode::Select:
    case Mode::AppendControlPoint:
    case Mode::InsertControlPoint:
    case Mode::AddColorPoint: {
        if (mMouseLeftButtonPressed) {
            onAction((Action) mMode, mTransformer->mapFromGuiToOpenGL(mMousePosition));
        }
        break;
    }
    default: {
        break;
    }
    }
}

void Controller::onMouseReleased(QMouseEvent *)
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
    default:
        break;
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

void Controller::zoom(float newZoomRatio, CustomVariant cursorPositionVariant)
{
    if (qFuzzyCompare(newZoomRatio, mProjectionParameters->zoomRatio))
        return;

    QPoint cursorPosition;
    float width = mOpenGLWidget->getWidth();
    float height = mOpenGLWidget->getHeight();

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

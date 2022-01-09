#include "Controller.h"
#include "Util.h"

Controller::Controller(QObject *parent)
    : QObject(parent)
    , mMode(Mode::Select)
    , mMouseLeftButtonPressed(false)
    , mMouseMiddleButtonPressed(false)
    , mMouseRightButtonPressed(false)
    , mMousePressedOnCurve(false)
    , mZoomStep(2.0f)
    , mKeyPressed(false)
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

    connect(mCentralWidget, &CentralWidget::action, this, &Controller::onAction);
    connect(mCurveWidget, &CurveWidget::action, this, &Controller::onAction);
    connect(mControlPointWidget, &ControlPointWidget::action, this, &Controller::onAction);
    connect(mColorPointWidget, &ColorPointWidget::action, this, &Controller::onAction);
    connect(mZoomWidget, &ZoomWidget::zoomRatioChanged, this, &Controller::onZoomRatioChanged);

    connect(mRenderSettingsWidget, &RenderSettingsWidget::action, this, &Controller::onAction);
    connect(mRenderSettingsWidget, &RenderSettingsWidget::renderModeChanged, mOpenGLWidget, &OpenGLWidget::onRenderModeChanged);
    connect(mRenderSettingsWidget, &RenderSettingsWidget::renderModeChanged, this, [=](RenderMode mode) {
        mRendererManager->onRenderModeChanged(mode);
        emit dirty(DirtType::OpenGL);
    });

    connect(mRenderSettingsWidget, &RenderSettingsWidget::colorRendererModeChanged, this, [=](ColorRendererMode mode) {
        mRendererManager->onColorRendererModeChanged(mode);
        emit dirty(DirtType::OpenGL);
    });

    connect(this, &Controller::zoomRatioChanged, mZoomWidget, &ZoomWidget::onZoomRatioChanged);
    connect(this, &Controller::dirty, mOpenGLWidget, &OpenGLWidget::onDirty);
    connect(this, &Controller::dirty, mModeWidget, &ModeWidget::onDirty);
    connect(this, &Controller::dirty, mCurveWidget, &CurveWidget::onDirty);
    connect(this, &Controller::dirty, mControlPointWidget, &ControlPointWidget::onDirty);
    connect(this, &Controller::dirty, mColorPointWidget, &ColorPointWidget::onDirty);

    connect(mCentralWidget, &CentralWidget::keyPressed, this, [=](Qt::Key key) {
        switch (key) {
        case Qt::Key_Control: {
            mKeyPressed = true;
            mPressedKey = key;
            mModeWidget->onModeChanged(Mode::AppendControlPoint);
            break;
        }

        case Qt::Key_Shift: {
            mKeyPressed = true;
            mPressedKey = key;
            mModeWidget->onModeChanged(Mode::InsertControlPoint);
            break;
        }

        case Qt::Key_Alt: {
            if (mCurveContainer->selectedCurve() != nullptr && mCurveContainer->selectedCurve()->getSize() >= 2) {
                mKeyPressed = true;
                mPressedKey = key;
                mModeWidget->onModeChanged(Mode::AddColorPoint);
            }
            break;
        }

        case Qt::Key_Delete: {
            if (mCurveContainer->selectedColorPoint())
                onAction(Action::RemoveColorPoint);
            else if (mCurveContainer->selectedControlPoint())
                onAction(Action::RemoveControlPoint);
            else if (mCurveContainer->selectedCurve())
                onAction(Action::RemoveCurve);

            break;
        }

        default:
            break;
        }
    });

    connect(mCentralWidget, &CentralWidget::keyReleased, this, [=](Qt::Key key) {
        if (mKeyPressed && mPressedKey == key) {
            mKeyPressed = false;
            mModeWidget->onModeChanged(Mode::Select);
        }
    });

    QVector<Curve *> curves = Util::readCurveDataFromXML("Resources/CurveData/zephyr.xml");
    mCurveContainer->addCurves(curves);
}

CentralWidget *Controller::centralWidget() const
{
    return mCentralWidget;
}

void Controller::onAction(Action action, CustomVariant value)
{
    switch (action) {
    case Action::ClearCanvas:
        mCurveContainer->setSelectedColorPoint(nullptr);
        mCurveContainer->setSelectedControlPoint(nullptr);
        mCurveContainer->setSelectedCurve(nullptr);
        mCurveContainer->clear();
        emit dirty(DirtType::OpenGL + DirtType::GUI);
        break;
    case Action::LoadFromXML: {
        QVector<Curve *> curves = Util::readCurveDataFromXML(value.toString());
        if (!curves.isEmpty()) {
            mCurveContainer->setSelectedColorPoint(nullptr);
            mCurveContainer->setSelectedControlPoint(nullptr);
            mCurveContainer->setSelectedCurve(nullptr);
            mCurveContainer->clear();
            mCurveContainer->addCurves(curves);
            emit dirty(DirtType::OpenGL + DirtType::GUI);
        }

        break;
    }
    case Action::SaveAsPNG: {
        ColorPoint *colorPoint = mCurveContainer->selectedColorPoint();
        ControlPoint *controlPoint = mCurveContainer->selectedControlPoint();
        Curve *curve = mCurveContainer->selectedCurve();

        mCurveContainer->setSelectedColorPoint(nullptr);
        mCurveContainer->setSelectedControlPoint(nullptr);
        mCurveContainer->setSelectedCurve(nullptr);

        mOpenGLWidget->save(value.toString());

        mCurveContainer->setSelectedColorPoint(colorPoint);
        mCurveContainer->setSelectedControlPoint(controlPoint);
        mCurveContainer->setSelectedCurve(curve);
        break;
    }
    case Action::UpdateRenderQuality:
        mRendererManager->onQualityChanged(value.toInt());
        emit dirty(DirtType::OpenGL);
        break;
    case Action::UpdateContourThickness:
        mRendererManager->onContourThicknessChanged(value.toFloat());
        emit dirty(DirtType::OpenGL);
        break;
    case Action::UpdateDiffusionWidth:
        mRendererManager->onDiffusionWidthChanged(value.toFloat());
        emit dirty(DirtType::OpenGL);
        break;
    case Action::UpdateContourColor:
        mRendererManager->onContourColorChanged(value.toVector4D());
        emit dirty(DirtType::OpenGL);
        break;
    case Action::UpdateSmoothIterations:
        mRendererManager->onSmoothIterationsChanged(value.toInt());
        emit dirty(DirtType::OpenGL);
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

            ColorPoint *colorPoint = new ColorPoint;
            colorPoint->setParent(mCurveContainer->selectedCurve());
            colorPoint->setPosition(parameter);
            colorPoint->setType(type);
            colorPoint->setColor(QVector4D(1, 1, 1, 1));

            if (mCurveContainer->selectedCurve()->addColorPoint(colorPoint)) {
                mCurveContainer->setSelectedColorPoint(colorPoint);
                mCurveContainer->setSelectedControlPoint(nullptr);
            } else {
                colorPoint->deleteLater();
            }

            emit dirty(DirtType::OpenGL + DirtType::GUI);
        }
        break;
    }
    case Action::RemoveColorPoint: {
        ColorPoint *selectedColorPoint = mCurveContainer->selectedColorPoint();

        Curve *selectedCurve = mCurveContainer->selectedCurve();
        if (selectedCurve && selectedColorPoint) {
            selectedCurve->removeColorPoint(selectedColorPoint);
            mCurveContainer->setSelectedColorPoint(nullptr);

            emit dirty(DirtType::OpenGL + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateColorPointColor: {
        ColorPoint *selectedColorPoint = mCurveContainer->selectedColorPoint();
        if (selectedColorPoint) {
            selectedColorPoint->setColor(value.toVector4D());

            emit dirty(DirtType::OpenGL + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateColorPointPosition: {
        ColorPoint *selectedColorPoint = mCurveContainer->selectedColorPoint();
        if (selectedColorPoint) {
            selectedColorPoint->setPosition(value.toFloat());

            emit dirty(DirtType::OpenGL + DirtType::GUI);
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
                emit dirty(DirtType::OpenGL + DirtType::GUI);
                return;
            }
        }

        Curve *selectedCurve = mCurveContainer->selectCurve(value.toVector2D(), 15 * mProjectionParameters->zoomRatio);
        mMousePressedOnCurve = selectedCurve ? true : false;
        mCurveContainer->setSelectedCurve(selectedCurve);

        emit dirty(DirtType::OpenGL + DirtType::GUI);
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

        emit dirty(DirtType::OpenGL + DirtType::GUI);
        break;
    }
    case Action::Move: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->selectedCurve()->translate(value.toVector2D());

            emit dirty(DirtType::OpenGL + DirtType::GUI);
        }
        break;
    }
    case Action::Pan: {
        QVector2D translation = value.toVector2D();
        mProjectionParameters->left += translation.x();
        mProjectionParameters->right += translation.x();
        mProjectionParameters->top += translation.y();
        mProjectionParameters->bottom += translation.y();

        emit dirty(DirtType::OpenGL + DirtType::GUI);
        break;
    }
    case Action::RemoveCurve: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->removeCurve(mCurveContainer->selectedCurve());
            mCurveContainer->setSelectedControlPoint(nullptr);
            mCurveContainer->setSelectedCurve(nullptr);
        }

        emit dirty(DirtType::OpenGL + DirtType::GUI);
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

        emit dirty(DirtType::OpenGL + DirtType::GUI);
        break;
    }
    case Action::UpdateControlPointPosition: {
        if (mCurveContainer->selectedControlPoint()) {
            mCurveContainer->selectedControlPoint()->setPosition(value.toVector2D());
            emit dirty(DirtType::OpenGL + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateControlPointXPosition: {
        if (mCurveContainer->selectedControlPoint()) {
            QVector2D position = mCurveContainer->selectedControlPoint()->position();
            position.setX(value.toFloat());
            mCurveContainer->selectedControlPoint()->setPosition(position);

            emit dirty(DirtType::OpenGL + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateControlPointYPosition: {
        if (mCurveContainer->selectedControlPoint()) {
            QVector2D position = mCurveContainer->selectedControlPoint()->position();
            position.setY(value.toFloat());
            mCurveContainer->selectedControlPoint()->setPosition(position);

            emit dirty(DirtType::OpenGL + DirtType::GUI);
        }
        break;
    }
    case Action::UpdateCurveZIndex: {
        if (mCurveContainer->selectedCurve()) {
            mCurveContainer->selectedCurve()->setZ(value.toInt());
            mCurveContainer->sortCurves();

            emit dirty(DirtType::OpenGL + DirtType::GUI);
        }
        break;
    }

    case Action::ZoomIn: {
        zoom(mProjectionParameters->zoomRatio / mZoomStep, value);

        emit dirty(DirtType::OpenGL + DirtType::GUI);
        break;
    }
    case Action::ZoomOut: {
        zoom(mProjectionParameters->zoomRatio * mZoomStep, value);

        emit dirty(DirtType::OpenGL + DirtType::GUI);
        break;
    }
    }
}

void Controller::onModeChanged(Mode mode)
{
    mMode = mode;

    emit dirty(DirtType::OpenGL);
}

void Controller::onZoomRatioChanged(float newZoomRatio)
{
    zoom(newZoomRatio);

    emit dirty(DirtType::OpenGL);
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
    mMouseLeftButtonPressed = event->button() == Qt::LeftButton;
    mMouseMiddleButtonPressed = event->button() == Qt::MiddleButton;
    mMouseRightButtonPressed = event->button() == Qt::RightButton;
    mModeBeforeMousePress = mMode;

    if (mMouseLeftButtonPressed) {
        switch (mMode) {
        case Mode::Select:
        case Mode::AppendControlPoint:
        case Mode::InsertControlPoint:
        case Mode::AddColorPoint: {
            {
                onAction((Action) mMode, mTransformer->mapFromGuiToOpenGL(mMousePosition));
            }
            break;
        }
        default: {
            break;
        }
        }
    }

    if (mMouseMiddleButtonPressed)
        mModeWidget->onModeChanged(Mode::Pan);

    if (mMouseRightButtonPressed)
        mModeWidget->onModeChanged(Mode::MoveCurve);
}

void Controller::onMouseReleased(QMouseEvent *event)
{
    if (mMouseMiddleButtonPressed) {
        mModeWidget->onModeChanged(mModeBeforeMousePress);
    }

    if (mMouseRightButtonPressed) {
        mModeWidget->onModeChanged(mModeBeforeMousePress);
    }

    mMouseLeftButtonPressed = false;
    mMouseMiddleButtonPressed = false;
    mMouseRightButtonPressed = false;
}

void Controller::onMouseMoved(QMouseEvent *event)
{
    switch (mMode) {
    case Mode::AddColorPoint:
        break;

    case Mode::Pan: {
        QVector2D translation = QVector2D(mMousePosition - event->pos()) * mProjectionParameters->zoomRatio;
        onAction(Action::Pan, translation);
        break;
    }

    case Mode::Select: {
        QVector2D position = mTransformer->mapFromGuiToOpenGL(event->pos());
        if (mMouseLeftButtonPressed) {
            if (mCurveContainer->selectedControlPoint()) {
                onAction(Action::UpdateControlPointPosition, position);
            } else if (mCurveContainer->selectedColorPoint()) {
                if (mCurveContainer->selectedCurve()) {
                    onAction(Action::UpdateColorPointPosition, mCurveContainer->selectedCurve()->parameterAt(position, 10000));
                }
            }
        }
        break;
    }

    case Mode::MoveCurve: {
        if (mMouseRightButtonPressed || mMouseLeftButtonPressed) {
            if (mCurveContainer->selectedCurve()) {
                QVector2D translation = QVector2D(event->pos() - mMousePosition) * mProjectionParameters->zoomRatio;
                onAction(Action::Move, translation);

            } else {
                QVector2D position = mTransformer->mapFromGuiToOpenGL(mMousePosition);
                onAction(Action::Select, position);
            }
        }

        break;
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

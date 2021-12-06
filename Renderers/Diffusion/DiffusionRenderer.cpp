#include "DiffusionRenderer.h"

#include <Curves/Bezier.h>

DiffusionRenderer::DiffusionRenderer()
    : mShader(nullptr)
    , mTicks(nullptr)
{}

DiffusionRenderer::~DiffusionRenderer()
{
    if (mShader)
        delete mShader;

    if (mTicks)
        delete mTicks;

    mShader = nullptr;
    mTicks = nullptr;
}

bool DiffusionRenderer::init()
{
    initializeOpenGLFunctions();

    mShader = new QOpenGLShaderProgram;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/Curve/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/Curve/FragmentShader.frag")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Diffusion/Curve/GeometryShader.geom") || !mShader->link()
        || !mShader->bind()) {
        qCritical() << mShader->log();
        return false;
    }

    // Uniform Variables
    mProjectionMatrixLocation = mShader->uniformLocation("projectionMatrix");
    mTicksDeltaLocation = mShader->uniformLocation("ticksDelta");
    mThicknessLocation = mShader->uniformLocation("thickness");

    mControlPointsLocation = mShader->uniformLocation("controlPoints");
    mControlPointsCountLocation = mShader->uniformLocation("controlPointsCount");

    mLeftColorsLocation = mShader->uniformLocation("leftColors");
    mLeftColorPositionsLocation = mShader->uniformLocation("leftColorPositions");
    mLeftColorsCountLocation = mShader->uniformLocation("leftColorsCount");

    mRightColorsLocation = mShader->uniformLocation("rightColors");
    mRightColorPositionsLocation = mShader->uniformLocation("rightColorPositions");
    mRightColorsCountLocation = mShader->uniformLocation("rightColorsCount");

    // Attribute Locations
    mShader->bindAttributeLocation("vs_Tick", 0);

    mTicks = new Ticks(0, 1, 2000);
    mTicks->create();

    mShader->release();

    return true;
}

void DiffusionRenderer::render(const QVector<Curve *> &curves, const QMatrix4x4 &projectionMatrix)
{
    mShader->bind();
    mTicks->bind();

    mShader->setUniformValue(mProjectionMatrixLocation, projectionMatrix);
    mShader->setUniformValue(mTicksDeltaLocation, mTicks->ticksDelta());

    for (int i = 0; i < curves.size(); ++i) {
        Bezier *curve = dynamic_cast<Bezier *>(curves[i]);

        if (curve == nullptr)
            continue;

        mShader->setUniformValue(mThicknessLocation, curve->thickness());

        // Control points
        QVector<QVector2D> controlPoints = curve->getControlPointPositions();
        mShader->setUniformValueArray(mControlPointsLocation, controlPoints.constData(), controlPoints.size());
        mShader->setUniformValue(mControlPointsCountLocation, (GLint) controlPoints.size());

        // Left colors
        QVector<QVector4D> leftColors = curve->getColors(Curve::Left);
        QVector<GLfloat> leftColorPositions = curve->getColorPositions(Curve::Left);
        mShader->setUniformValueArray(mLeftColorsLocation, leftColors.constData(), leftColors.size());
        mShader->setUniformValueArray(mLeftColorPositionsLocation, leftColorPositions.constData(), leftColorPositions.size(), 1);
        mShader->setUniformValue(mLeftColorsCountLocation, (GLint) leftColors.size());

        // Right colors
        QVector<QVector4D> rightColors = curve->getColors(Curve::Right);
        QVector<GLfloat> rightColorPositions = curve->getColorPositions(Curve::Right);
        mShader->setUniformValueArray(mRightColorsLocation, rightColors.constData(), rightColors.size());
        mShader->setUniformValueArray(mRightColorPositionsLocation, rightColorPositions.constData(), rightColorPositions.size(), 1);
        mShader->setUniformValue(mRightColorsCountLocation, (GLint) rightColors.size());

        glDrawArrays(GL_POINTS, 0, mTicks->size());
    }

    mTicks->release();
    mShader->release();
}

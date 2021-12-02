#include "BezierDiffusionRenderer.h"

#include <Curves/Bezier.h>

BezierDiffusionRenderer::BezierDiffusionRenderer()
    : mShader(nullptr)
    , mInitialized(false)
{}

BezierDiffusionRenderer::~BezierDiffusionRenderer()
{
    mTicksBuffer.destroy();
    mTicksVertexArray.destroy();

    if (mShader)
        delete mShader;

    mShader = nullptr;
}

bool BezierDiffusionRenderer::initialize()
{
    initializeOpenGLFunctions();

    mShader = new QOpenGLShaderProgram;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Bezier/Diffusion/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Bezier/Diffusion/FragmentShader.frag")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Bezier/Diffusion/GeometryShader.geom")
        || !mShader->link() || !mShader->bind()) {
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
    mShader->bindAttributeLocation("vertex", 0);

    // Ticks
    mTicks = QVector<float>(2000, 0.0);
    for (int i = 0; i < mTicks.size(); ++i)
        mTicks[i] = i / static_cast<float>(mTicks.size());

    mTicksDelta = 1.0f / static_cast<float>(mTicks.size());

    // VAO and VBO
    {
        mTicksVertexArray.create();
        mTicksVertexArray.bind();

        mTicksBuffer.create();
        mTicksBuffer.bind();
        mTicksBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        mTicksBuffer.allocate(mTicks.constData(), mTicks.size() * sizeof(GLfloat));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
        mTicksBuffer.release();

        mTicksVertexArray.release();
    }

    return mInitialized = true;
}

void BezierDiffusionRenderer::render(QVector<Curve *> curves)
{
    if (!mInitialized)
        return;

    for (int i = 0; i < curves.size(); ++i) {
        Bezier *curve = dynamic_cast<Bezier *>(curves[i]);

        if (curve == nullptr)
            continue;

        mShader->bind();
        mShader->setUniformValue(mProjectionMatrixLocation, mProjectionMatrix);

        mShader->setUniformValue(mTicksDeltaLocation, mTicksDelta);
        mShader->setUniformValue(mThicknessLocation, curve->thickness());

        // Control points
        QVector<QVector2D> controlPoints = curve->getControlPointPositions();
        mShader->setUniformValueArray(mControlPointsLocation, controlPoints.constData(), controlPoints.size());
        mShader->setUniformValue(mControlPointsCountLocation, controlPoints.size());

        // Left colors
        QVector<QVector4D> leftColors = curve->getColors(Curve::Left);
        QVector<GLfloat> leftColorPositions = curve->getColorPositions(Curve::Left);
        mShader->setUniformValueArray(mLeftColorsLocation, leftColors.constData(), leftColors.size());
        mShader->setUniformValueArray(mLeftColorPositionsLocation,
                                      leftColorPositions.constData(),
                                      leftColorPositions.size(),
                                      1);
        mShader->setUniformValue(mLeftColorsCountLocation, leftColors.size());

        // Right colors
        QVector<QVector4D> rightColors = curve->getColors(Curve::Right);
        QVector<GLfloat> rightColorPositions = curve->getColorPositions(Curve::Right);
        mShader->setUniformValueArray(mRightColorsLocation, rightColors.constData(), rightColors.size());
        mShader->setUniformValueArray(mRightColorPositionsLocation,
                                      rightColorPositions.constData(),
                                      rightColorPositions.size(),
                                      1);
        mShader->setUniformValue(mRightColorsCountLocation, rightColors.size());

        mTicksVertexArray.bind();
        glDrawArrays(GL_POINTS, 0, mTicks.size());
        mTicksVertexArray.release();

        mShader->release();
    }
}

void BezierDiffusionRenderer::setProjectionMatrix(const QMatrix4x4 &newProjectionMatrix)
{
    mProjectionMatrix = newProjectionMatrix;
}

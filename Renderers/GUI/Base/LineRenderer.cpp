#include "LineRenderer.h"

LineRenderer::LineRenderer()
    : mShader(nullptr)
    , mDenseTicks(nullptr)
    , mRareTicks(nullptr)
{}

LineRenderer::~LineRenderer()
{
    if (mShader)
        delete mShader;

    if (mDenseTicks)
        delete mDenseTicks;

    if (mRareTicks)
        delete mRareTicks;

    mShader = nullptr;
    mDenseTicks = nullptr;
    mRareTicks = nullptr;
}

bool LineRenderer::initialize()
{
    initializeOpenGLFunctions();

    mShader = new QOpenGLShaderProgram;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Line/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Line/FragmentShader.frag")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Line/GeometryShader.geom") || !mShader->link() || !mShader->bind()) {
        qCritical() << this << mShader->log();
        return false;
    }

    // Uniforms Variables
    mProjectionMatrixLocation = mShader->uniformLocation("projectionMatrix");
    mStartingPointLocation = mShader->uniformLocation("startingPoint");
    mEndPointLocation = mShader->uniformLocation("endPoint");
    mColorLocation = mShader->uniformLocation("color");
    mThicknessLocation = mShader->uniformLocation("thickness");
    mRareTicksDeltaLocation = mShader->uniformLocation("rareTicksDelta");
    mDenseTicksDeltaLocation = mShader->uniformLocation("denseTicksDelta");
    mLineStyleLocation = mShader->uniformLocation("lineStyle");
    mLineLengthLocation = mShader->uniformLocation("lineLength");
    mDashLengthLocation = mShader->uniformLocation("dashLength");
    mGapLengthLocation = mShader->uniformLocation("gapLength");

    // Vertex Attribute Location
    mShader->bindAttributeLocation("vs_Tick", 0);

    // Ticks
    mRareTicks = new Ticks(0, 1, 10);
    mRareTicks->create();

    mDenseTicks = new Ticks(0, 1, 1000);
    mDenseTicks->create();

    return true;
}

void LineRenderer::render(const RenderParameters &params)
{
    mShader->bind();

    mShader->setUniformValue(mProjectionMatrixLocation, mProjectionMatrix);
    mShader->setUniformValue(mRareTicksDeltaLocation, mRareTicks->ticksDelta());
    mShader->setUniformValue(mDenseTicksDeltaLocation, mDenseTicks->ticksDelta());

    mShader->setUniformValue(mStartingPointLocation, params.startingPoint);
    mShader->setUniformValue(mEndPointLocation, params.endPoint);
    mShader->setUniformValue(mColorLocation, params.color);
    mShader->setUniformValue(mThicknessLocation, params.thickness);
    mShader->setUniformValue(mLineStyleLocation, params.lineStyle);
    mShader->setUniformValue(mLineLengthLocation, params.startingPoint.distanceToPoint(params.endPoint));
    mShader->setUniformValue(mDashLengthLocation, params.dashLength);
    mShader->setUniformValue(mGapLengthLocation, params.gapLength);

    if (params.lineStyle == LineStyle::Solid) {
        mRareTicks->bind();
        glDrawArrays(GL_POINTS, 0, mRareTicks->size());
        mRareTicks->release();

    } else if (params.lineStyle == LineStyle::Dashed) {
        mDenseTicks->bind();
        glDrawArrays(GL_POINTS, 0, mDenseTicks->size());
        mDenseTicks->release();
    }

    mShader->release();
}

void LineRenderer::setProjectionMatrix(const QMatrix4x4 &newMatrix)
{
    mProjectionMatrix = newMatrix;
}

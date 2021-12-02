#include "LineRenderer.h"

LineRenderer::LineRenderer()
    : mShader(nullptr)
    , mInitialized(false)
{}

LineRenderer::~LineRenderer()
{
    mRareTicksBuffer.destroy();
    mRareTicksVertexArray.destroy();

    mDenseTicksBuffer.destroy();
    mDenseTicksVertexArray.destroy();

    if (mShader)
        delete mShader;

    mShader = nullptr;
}

bool LineRenderer::initialize()
{
    initializeOpenGLFunctions();

    mShader = new QOpenGLShaderProgram;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Line/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Line/FragmentShader.frag")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Line/GeometryShader.geom")
        || !mShader->link() || !mShader->bind()) {
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
    mShader->bindAttributeLocation("vertex", 0);

    // Rare Ticks
    {
        mRareTicksVertexArray.create();
        mRareTicksVertexArray.bind();

        mRareTicks = QVector<float>(10, 0);
        for (int i = 0; i < mRareTicks.size(); i++)
            mRareTicks[i] = static_cast<float>(i) / mRareTicks.size();

        mRareTicksDelta = 1.0f / mRareTicks.size();

        mRareTicksBuffer.create();
        mRareTicksBuffer.bind();
        mRareTicksBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        mRareTicksBuffer.allocate(mRareTicks.constData(), mRareTicks.size() * sizeof(GL_FLOAT));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
        mRareTicksBuffer.release();

        mRareTicksVertexArray.release();
    }

    // Dense Ticks
    {
        mDenseTicksVertexArray.create();
        mDenseTicksVertexArray.bind();

        mDenseTicks = QVector<float>(1000, 0);
        for (int i = 0; i < mDenseTicks.size(); i++)
            mDenseTicks[i] = static_cast<float>(i) / mDenseTicks.size();

        mDenseTicksDelta = 1.0f / mDenseTicks.size();

        mDenseTicksBuffer.create();
        mDenseTicksBuffer.bind();
        mDenseTicksBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        mDenseTicksBuffer.allocate(mDenseTicks.constData(), mDenseTicks.size() * sizeof(GL_FLOAT));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
        mDenseTicksBuffer.release();

        mDenseTicksVertexArray.release();
    }

    return mInitialized = true;
}

void LineRenderer::render(const RenderParameters &params)
{
    if (!mInitialized)
        return;

    mShader->bind();

    mShader->setUniformValue(mProjectionMatrixLocation, mProjectionMatrix);
    mShader->setUniformValue(mRareTicksDeltaLocation, mRareTicksDelta);
    mShader->setUniformValue(mDenseTicksDeltaLocation, mDenseTicksDelta);

    mShader->setUniformValue(mStartingPointLocation, params.startingPoint);
    mShader->setUniformValue(mEndPointLocation, params.endPoint);
    mShader->setUniformValue(mColorLocation, params.color);
    mShader->setUniformValue(mThicknessLocation, params.thickness);
    mShader->setUniformValue(mLineStyleLocation, params.lineStyle);
    mShader->setUniformValue(mLineLengthLocation, params.startingPoint.distanceToPoint(params.endPoint));
    mShader->setUniformValue(mDashLengthLocation, params.dashLength);
    mShader->setUniformValue(mGapLengthLocation, params.gapLength);

    if (params.lineStyle == LineStyle::Solid) {
        mRareTicksVertexArray.bind();
        glDrawArrays(GL_POINTS, 0, mRareTicks.size());
        mRareTicksVertexArray.release();

    } else if (params.lineStyle == LineStyle::Dashed) {
        mDenseTicksVertexArray.bind();
        glDrawArrays(GL_POINTS, 0, mDenseTicks.size());
        mDenseTicksVertexArray.release();
    }

    mShader->release();
}

void LineRenderer::setProjectionMatrix(const QMatrix4x4 &newMatrix) { mProjectionMatrix = newMatrix; }

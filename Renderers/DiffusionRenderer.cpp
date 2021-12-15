#include "DiffusionRenderer.h"

#include <Curves/Bezier.h>

DiffusionRenderer::DiffusionRenderer()
    : mQuality(1.0f)
    , mSmoothIterations(40)
    , mDiffusionWidth(4.0f)
    , mInit(false)
{}

DiffusionRenderer::~DiffusionRenderer() {}

bool DiffusionRenderer::init()
{
    initializeOpenGLFunctions();

    // Color Curve
    {
        mColorCurveShader = new QOpenGLShaderProgram;
        if (!mColorCurveShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/ColorCurve/VertexShader.vert")
            || !mColorCurveShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/ColorCurve/FragmentShader.frag")
            || !mColorCurveShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Diffusion/ColorCurve/GeometryShader.geom")
            || !mColorCurveShader->link() || !mColorCurveShader->bind()) {
            qCritical() << mColorCurveShader->log();
            return false;
        }

        // Uniform Variables

        mColorCurveLocations.insert("projectionMatrix", mColorCurveShader->uniformLocation("projectionMatrix"));
        mColorCurveLocations.insert("ticksDelta", mColorCurveShader->uniformLocation("ticksDelta"));
        mColorCurveLocations.insert("diffusionWidth", mColorCurveShader->uniformLocation("diffusionWidth"));

        mColorCurveLocations.insert("controlPoints", mColorCurveShader->uniformLocation("controlPoints"));
        mColorCurveLocations.insert("controlPointsCount", mColorCurveShader->uniformLocation("controlPointsCount"));

        mColorCurveLocations.insert("leftColors", mColorCurveShader->uniformLocation("leftColors"));
        mColorCurveLocations.insert("leftColorPositions", mColorCurveShader->uniformLocation("leftColorPositions"));
        mColorCurveLocations.insert("leftColorsCount", mColorCurveShader->uniformLocation("leftColorsCount"));

        mColorCurveLocations.insert("rightColors", mColorCurveShader->uniformLocation("rightColors"));
        mColorCurveLocations.insert("rightColorPositions", mColorCurveShader->uniformLocation("rightColorPositions"));
        mColorCurveLocations.insert("rightColorsCount", mColorCurveShader->uniformLocation("rightColorsCount"));

        qDebug() << "ColorCurve Shader Locations:" << mColorCurveLocations;

        // Attribute Locations
        mColorCurveShader->bindAttributeLocation("vs_Tick", 0);

        mTicks = new Ticks(0, 1.0, 100);
        mTicks->create();

        mColorCurveShader->release();
    }

    // Downsampler
    {
        mDownsamplerShader = new QOpenGLShaderProgram;

        if (!mDownsamplerShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/Downsampler/VertexShader.vert")
            || !mDownsamplerShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/Downsampler/FragmentShader.frag")
            || !mDownsamplerShader->link() || !mDownsamplerShader->bind()) {
            qCritical() << mDownsamplerShader->log();
            return false;
        }

        mDownsamplerLocations.insert("sourceTexture", mDownsamplerShader->uniformLocation("sourceTexture"));
        mDownsamplerLocations.insert("targetWidth", mDownsamplerShader->uniformLocation("targetWidth"));
        mDownsamplerLocations.insert("targetHeight", mDownsamplerShader->uniformLocation("targetHeight"));

        qDebug() << "Downsampler Shader Locations:" << mDownsamplerLocations;

        mDownsamplerShader->bindAttributeLocation("vs_Position", 0);
        mDownsamplerShader->bindAttributeLocation("vs_TextureCoords", 1);
        mDownsamplerShader->release();
    }

    // Upsampler
    {
        mUpsamplerShader = new QOpenGLShaderProgram;

        if (!mUpsamplerShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/Upsampler/VertexShader.vert")
            || !mUpsamplerShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/Upsampler/FragmentShader.frag")
            || !mUpsamplerShader->link() || !mUpsamplerShader->bind()) {
            qCritical() << mUpsamplerShader->log();
            return false;
        }

        mUpsamplerLocations.insert("sourceTexture", mUpsamplerShader->uniformLocation("sourceTexture"));
        mUpsamplerLocations.insert("targetTexture", mUpsamplerShader->uniformLocation("targetTexture"));

        qDebug() << "Upsampler Shader Locations:" << mUpsamplerLocations;

        mUpsamplerShader->bindAttributeLocation("vs_Position", 0);
        mUpsamplerShader->bindAttributeLocation("vs_TextureCoords", 1);
        mUpsamplerShader->release();
    }

    // Smoother
    {
        mSmootherShader = new QOpenGLShaderProgram;

        if (!mSmootherShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/Smoother/VertexShader.vert")
            || !mSmootherShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/Smoother/FragmentShader.frag")
            || !mSmootherShader->link() || !mSmootherShader->bind()) {
            qCritical() << mSmootherShader->log();
            return false;
        }

        mSmootherLocations.insert("sourceTexture", mSmootherShader->uniformLocation("sourceTexture"));
        mSmootherLocations.insert("targetTexture", mSmootherShader->uniformLocation("targetTexture"));
        mSmootherLocations.insert("targetWidth", mSmootherShader->uniformLocation("targetWidth"));
        mSmootherLocations.insert("targetHeight", mSmootherShader->uniformLocation("targetHeight"));

        qDebug() << "Smoother Shader Locations:" << mSmootherLocations;

        mSmootherShader->bindAttributeLocation("vs_Position", 0);
        mSmootherShader->bindAttributeLocation("vs_TextureCoords", 1);

        mSmootherShader->release();
    }

    // Blur
    {
        mBlurShader = new QOpenGLShaderProgram;

        if (!mBlurShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/Blur/VertexShader.vert")
            || !mBlurShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/Blur/FragmentShader.frag") || !mBlurShader->link()
            || !mBlurShader->bind()) {
            qCritical() << mBlurShader->log();
            return false;
        }

        mBlurShaderLocations.insert("sourceTexture", mBlurShader->uniformLocation("sourceTexture"));
        mBlurShaderLocations.insert("targetWidth", mBlurShader->uniformLocation("targetWidth"));
        mBlurShaderLocations.insert("targetHeight", mBlurShader->uniformLocation("targetHeight"));

        qDebug() << "Blur Shader Locations:" << mBlurShaderLocations;

        mBlurShader->bindAttributeLocation("vs_Position", 0);
        mBlurShader->bindAttributeLocation("vs_TextureCoords", 1);
        mBlurShader->release();
    }

    mQuads = new Quads;
    mQuads->create();

    return mInit = true;
}

void DiffusionRenderer::renderColorCurves(const QVector<Curve *> &curves, const QMatrix4x4 &projectionMatrix)
{
    if (!mInit)
        return;

    mColorCurveShader->bind();
    mTicks->bind();

    mColorCurveShader->setUniformValue(mColorCurveLocations.value("projectionMatrix"), projectionMatrix);
    mColorCurveShader->setUniformValue(mColorCurveLocations.value("ticksDelta"), mTicks->ticksDelta());

    for (int i = 0; i < curves.size(); ++i) {
        Bezier *curve = dynamic_cast<Bezier *>(curves[i]);

        if (curve == nullptr)
            continue;

        curve->scale(mQuality);

        mColorCurveShader->setUniformValue("diffusionWidth", mDiffusionWidth);

        // Control points
        QVector<QVector2D> controlPoints = curve->getControlPointPositions();
        mColorCurveShader->setUniformValueArray(mColorCurveLocations.value("controlPoints"), controlPoints.constData(), controlPoints.size());
        mColorCurveShader->setUniformValue(mColorCurveLocations.value("controlPointsCount"), (GLint) controlPoints.size());

        // Left colors
        QVector<QVector4D> leftColors = curve->getLeftColors();
        QVector<float> leftColorPositions = curve->getLeftColorPositions();

        mColorCurveShader->setUniformValueArray(mColorCurveLocations.value("leftColors"), leftColors.constData(), leftColors.size());
        mColorCurveShader->setUniformValueArray(mColorCurveLocations.value("leftColorPositions"),
                                                leftColorPositions.constData(),
                                                leftColorPositions.size(),
                                                1);
        mColorCurveShader->setUniformValue(mColorCurveLocations.value("leftColorsCount"), (GLint) leftColors.size());

        // Right colors
        QVector<QVector4D> rightColors = curve->getRightColors();
        QVector<float> rightColorPositions = curve->getRightColorPositions();
        mColorCurveShader->setUniformValueArray(mColorCurveLocations.value("rightColors"), rightColors.constData(), rightColors.size());
        mColorCurveShader->setUniformValueArray(mColorCurveLocations.value("rightColorPositions"),
                                                rightColorPositions.constData(),
                                                rightColorPositions.size(),
                                                1);
        mColorCurveShader->setUniformValue(mColorCurveLocations.value("rightColorsCount"), (GLint) rightColors.size());

        glDrawArrays(GL_POINTS, 0, mTicks->size());

        curve->scale(1.0f / mQuality);
    }

    mTicks->release();
    mColorCurveShader->release();
}

void DiffusionRenderer::blur(const Parameters &parameters)
{
    if (!mInit)
        return;

    mBlurShader->bind();

    mBlurShader->setUniformValue(mBlurShaderLocations.value("sourceTexture"), GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parameters.sourceTexture);

    mBlurShader->setUniformValue(mBlurShaderLocations.value("targetWidth"), parameters.targetWidth);
    mBlurShader->setUniformValue(mBlurShaderLocations.value("targetHeight"), parameters.targetHeight);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mBlurShader->release();
}

void DiffusionRenderer::downsample(const Parameters &parameters)
{
    if (!mInit)
        return;

    mDownsamplerShader->bind();

    mDownsamplerShader->setUniformValue(mDownsamplerLocations.value("sourceTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parameters.sourceTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    mDownsamplerShader->setUniformValue(mDownsamplerLocations.value("targetWidth"), parameters.targetWidth);
    mDownsamplerShader->setUniformValue(mDownsamplerLocations.value("targetHeight"), parameters.targetHeight);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mDownsamplerShader->release();
}

void DiffusionRenderer::upsample(const Parameters &parameters)
{
    if (!mInit)
        return;

    mUpsamplerShader->bind();

    mUpsamplerShader->setUniformValue(mUpsamplerLocations.value("sourceTexture"), 0);
    mUpsamplerShader->setUniformValue(mUpsamplerLocations.value("tagetTexture"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parameters.sourceTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, parameters.targetTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mUpsamplerShader->release();
}

void DiffusionRenderer::smooth(const Parameters &parameters)
{
    if (!mInit)
        return;

    mSmootherShader->bind();
    mQuads->bind();

    mSmootherShader->setUniformValue(mSmootherLocations.value("targetWidth"), parameters.targetWidth);
    mSmootherShader->setUniformValue(mSmootherLocations.value("targetHeight"), parameters.targetHeight);

    for (int i = 0; i < mSmoothIterations; ++i) {
        mSmootherShader->setUniformValue(mSmootherLocations.value("sourceTexture"), 3);
        mSmootherShader->setUniformValue(mSmootherLocations.value("targetTexture"), 4);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, parameters.sourceTexture);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, parameters.targetTexture);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    mQuads->release();
    mSmootherShader->release();
}

int DiffusionRenderer::smoothIterations() const
{
    return mSmoothIterations;
}

void DiffusionRenderer::setSmoothIterations(int newSmoothIterations)
{
    mSmoothIterations = newSmoothIterations;
}

float DiffusionRenderer::diffusionWidth() const
{
    return mDiffusionWidth;
}

void DiffusionRenderer::setDiffusionWidth(float newDiffusionWidth)
{
    mDiffusionWidth = newDiffusionWidth;
}

int DiffusionRenderer::quality() const
{
    return mQuality;
}

void DiffusionRenderer::setQuality(int newQuality)
{
    mQuality = newQuality;
}

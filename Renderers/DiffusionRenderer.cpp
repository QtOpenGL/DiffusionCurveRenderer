#include "DiffusionRenderer.h"

#include <Curves/Bezier.h>

DiffusionRenderer::DiffusionRenderer()
    : mInit(false)
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
        mColorCurveLocations.insert("thickness", mColorCurveShader->uniformLocation("thickness"));

        mColorCurveLocations.insert("controlPoints", mColorCurveShader->uniformLocation("controlPoints"));
        mColorCurveLocations.insert("controlPointsCount", mColorCurveShader->uniformLocation("controlPointsCount"));

        mColorCurveLocations.insert("leftColors", mColorCurveShader->uniformLocation("leftColors"));
        mColorCurveLocations.insert("leftColorPositions", mColorCurveShader->uniformLocation("leftColorPositions"));
        mColorCurveLocations.insert("leftColorsCount", mColorCurveShader->uniformLocation("leftColorsCount"));

        mColorCurveLocations.insert("rightColors", mColorCurveShader->uniformLocation("rightColors"));
        mColorCurveLocations.insert("rightColorPositions", mColorCurveShader->uniformLocation("rightColorPositions"));
        mColorCurveLocations.insert("rightColorsCount", mColorCurveShader->uniformLocation("rightColorsCount"));

        // Attribute Locations
        mColorCurveShader->bindAttributeLocation("vs_Tick", 0);

        mTicks = new Ticks(0, 0.99, 2000);
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

        mSmootherLocations.insert("constrainedTexture", mSmootherShader->uniformLocation("constrainedTexture"));
        mSmootherLocations.insert("targetTexture", mSmootherShader->uniformLocation("targetTexture"));
        mSmootherLocations.insert("targetWidth", mSmootherShader->uniformLocation("targetWidth"));
        mSmootherLocations.insert("targetHeight", mSmootherShader->uniformLocation("targetHeight"));

        mSmootherShader->bindAttributeLocation("vs_Position", 0);
        mSmootherShader->bindAttributeLocation("vs_TextureCoords", 1);
        mSmootherShader->release();
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

        mColorCurveShader->setUniformValue("thickness", curve->thickness());

        // Control points
        QVector<QVector2D> controlPoints = curve->getControlPointPositions();
        mColorCurveShader->setUniformValueArray(mColorCurveLocations.value("controlPoints"), controlPoints.constData(), controlPoints.size());
        mColorCurveShader->setUniformValue(mColorCurveLocations.value("controlPointsCount"), (GLint) controlPoints.size());

        // Left colors
        QVector<QVector4D> leftColors = curve->getColors(Curve::Left);
        QVector<GLfloat> leftColorPositions = curve->getColorPositions(Curve::Left);

        mColorCurveShader->setUniformValueArray(mColorCurveLocations.value("leftColors"), leftColors.constData(), leftColors.size());
        mColorCurveShader->setUniformValueArray(mColorCurveLocations.value("leftColorPositions"),
                                                leftColorPositions.constData(),
                                                leftColorPositions.size(),
                                                1);
        mColorCurveShader->setUniformValue(mColorCurveLocations.value("leftColorsCount"), (GLint) leftColors.size());

        // Right colors
        QVector<QVector4D> rightColors = curve->getColors(Curve::Right);
        QVector<GLfloat> rightColorPositions = curve->getColorPositions(Curve::Right);
        mColorCurveShader->setUniformValueArray(mColorCurveLocations.value("rightColors"), rightColors.constData(), rightColors.size());
        mColorCurveShader->setUniformValueArray(mColorCurveLocations.value("rightColorPositions"),
                                                rightColorPositions.constData(),
                                                rightColorPositions.size(),
                                                1);
        mColorCurveShader->setUniformValue(mColorCurveLocations.value("rightColorsCount"), (GLint) rightColors.size());

        glDrawArrays(GL_POINTS, 0, mTicks->size());
    }

    mTicks->release();
    mColorCurveShader->release();
}

void DiffusionRenderer::downsample(GLuint sourceTexture, float targetWidth, float targetHeight)
{
    if (!mInit)
        return;

    mDownsamplerShader->bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    mDownsamplerShader->setUniformValue(mDownsamplerLocations.value("sourceTexture"), GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sourceTexture);

    mDownsamplerShader->setUniformValue(mDownsamplerLocations.value("targetWidth"), targetWidth);
    mDownsamplerShader->setUniformValue(mDownsamplerLocations.value("targetHeight"), targetHeight);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mDownsamplerShader->release();
}

void DiffusionRenderer::upsample(GLuint sourceTexture, GLuint targetTexture, float targetWidth, float targetHeight)
{
    if (!mInit)
        return;

    mUpsamplerShader->bind();

    mUpsamplerShader->setUniformValue(mUpsamplerLocations.value("sourceTexture"), 0);
    mUpsamplerShader->setUniformValue(mUpsamplerLocations.value("tagetTexture"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sourceTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, targetTexture);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mUpsamplerShader->release();
}

void DiffusionRenderer::smooth(GLuint constrainedTexture, GLuint targetTexture, float targetWidth, float targetHeight, int iterations)
{
    if (!mInit)
        return;
    mSmootherShader->bind();
    mQuads->bind();
    for (int i = 0; i < iterations; ++i) {
        mUpsamplerShader->setUniformValue(mSmootherLocations.value("constrainedTexture"), 3);
        mUpsamplerShader->setUniformValue(mSmootherLocations.value("targetTexture"), 4);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, constrainedTexture);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, targetTexture);

        mSmootherShader->setUniformValue(mSmootherLocations.value("targetWidth"), targetWidth);
        mSmootherShader->setUniformValue(mSmootherLocations.value("targetHeight"), targetHeight);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    mQuads->release();
    mSmootherShader->release();
}

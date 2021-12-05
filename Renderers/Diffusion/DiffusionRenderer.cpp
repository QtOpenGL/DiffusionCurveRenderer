#include "DiffusionRenderer.h"

#include <Curves/Bezier.h>

DiffusionRenderer::DiffusionRenderer(float bufferWidth, float bufferHeight)
    : mBufferWidth(bufferWidth)
    , mBufferHeight(bufferHeight)
    , mShader(nullptr)
    , mMultisampleFramebuffer(nullptr)
    , mDownsampledFramebuffer(nullptr)
    , mTicks(nullptr)
{}

DiffusionRenderer::~DiffusionRenderer()
{
    if (mShader)
        delete mShader;

    if (mMultisampleFramebuffer)
        delete mMultisampleFramebuffer;

    if (mDownsampledFramebuffer)
        delete mDownsampledFramebuffer;

    if (mTicks)
        delete mTicks;

    mShader = nullptr;
    mMultisampleFramebuffer = nullptr;
    mDownsampledFramebuffer = nullptr;
    mTicks = nullptr;
}

bool DiffusionRenderer::initialize()
{
    initializeOpenGLFunctions();

    mMultisampleFrambufferFormat.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    mMultisampleFrambufferFormat.setMipmap(true);
    mMultisampleFrambufferFormat.setSamples(32);
    mMultisampleFrambufferFormat.setTextureTarget(GL_TEXTURE_2D);
    mMultisampleFrambufferFormat.setInternalTextureFormat(GL_RGBA8);
    mMultisampleFramebuffer = new QOpenGLFramebufferObject(mBufferWidth, mBufferHeight, mMultisampleFrambufferFormat);

    mDownsampledFrambufferFormat.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    mDownsampledFrambufferFormat.setMipmap(true);
    mDownsampledFrambufferFormat.setTextureTarget(GL_TEXTURE_2D);
    mDownsampledFrambufferFormat.setInternalTextureFormat(GL_RGBA8);
    mDownsampledFramebuffer = new QOpenGLFramebufferObject(mBufferWidth, mBufferHeight, mDownsampledFrambufferFormat);

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

void DiffusionRenderer::render(const QVector<Curve *> &curves)
{
    mShader->bind();
    mTicks->bind();
    mMultisampleFramebuffer->bind();
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mShader->setUniformValue(mProjectionMatrixLocation, mProjectionMatrix);
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

    //static int i = 0;
    //mMultisampleFramebuffer->toImage().save(QString("MultisampleFramebuffer %1.png").arg(i));

    QOpenGLFramebufferObject::blitFramebuffer(mDownsampledFramebuffer, mMultisampleFramebuffer);

    //mDownsampledFramebuffer->toImage().save(QString("DownsampledFramebuffer %1.png").arg(i++));

    //    QOpenGLFramebufferObject *target,
    //    const QRect &targetRect,
    //    QOpenGLFramebufferObject *source,
    //    const QRect &sourceRect,
    //    GLbitfield buffers,
    //    GLenum filter,
    //    int readColorAttachmentIndex,
    //    int drawColorAttachmentIndex,
    //    QOpenGLFramebufferObject::FramebufferRestorePolicy restorePolicy

    mMultisampleFramebuffer->release();
    mTicks->release();
    mShader->release();
}

GLuint DiffusionRenderer::getTexture() const
{
    return mDownsampledFramebuffer->texture();
}

void DiffusionRenderer::setProjectionMatrix(const QMatrix4x4 &newProjectionMatrix)
{
    mProjectionMatrix = newProjectionMatrix;
}

float DiffusionRenderer::bufferWidth() const
{
    return mBufferWidth;
}

float DiffusionRenderer::bufferHeight() const
{
    return mBufferHeight;
}

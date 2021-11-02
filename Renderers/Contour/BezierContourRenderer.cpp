#include "BezierContourRenderer.h"

BezierContourRenderer::BezierContourRenderer(QObject *parent) :
    QObject(parent),
    mShader(nullptr),
    mMode(ModeWidget::Select),
    mZoomRatio(1.0f)
{

}

BezierContourRenderer::~BezierContourRenderer()
{
    mTicksBuffer.destroy();
    mTicksVertexArray.destroy();

    if(mShader)
        delete mShader;

    mShader = nullptr;
}

bool BezierContourRenderer::initialize()
{
    initializeOpenGLFunctions();
    mShader = new QOpenGLShaderProgram;

    if(!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Bezier/Contour/VertexShader.vert") ||
            !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Bezier/Contour/FragmentShader.frag") ||
            !mShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Bezier/Contour/GeometryShader.geom") ||
            !mShader->link() ||
            !mShader->bind())
    {
        qCritical() << this << mShader->log();
        return false;
    }

    // Uniforms
    mProjectionMatrixLocation = mShader->uniformLocation("projectionMatrix");
    mThicknessLocation = mShader->uniformLocation("thickness");
    mColorLocation = mShader->uniformLocation("color");
    mTicksDeltaLocation = mShader->uniformLocation("ticksDelta");
    mControlPointsCountLocation = mShader->uniformLocation("controlPointsCount");

    // Locations
    mShader->bindAttributeLocation("vertex", 0);

    // Shader Storage Buffer Object;
    glGenBuffers(1, &mControlPointsBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mControlPointsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mControlPointsBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, Constants::MAX_CONTROL_POINT_COUNT * sizeof(QVector2D), nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Ticks
    mTicks = QVector<float>(200, 0.0);
    for(int i = 0; i < mTicks.size(); ++i)
        mTicks[i] = i / static_cast<float>(mTicks.size());

    mTicksDelta = 1.0f / static_cast<float>(mTicks.size());

    // VAO and VBO
    mTicksVertexArray.create();
    QOpenGLVertexArrayObject::Binder binder(&mTicksVertexArray);

    mTicksBuffer.create();
    mTicksBuffer.bind();
    mTicksBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    mTicksBuffer.allocate(mTicks.constData(), mTicks.size() * sizeof(GLfloat));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    mTicksBuffer.release();

    return true;
}

void BezierContourRenderer::render(QVector<Curve*> curves)
{
    QVector<Curve*> orderedCurves = orderCurves(curves);

    bool highlightOnlySelectedCurve = false;

    for(int i = 0; i < orderedCurves.size(); ++i)
    {
        if(orderedCurves.at(i)->selected())
        {
            highlightOnlySelectedCurve = true;
            break;
        }

    }

    switch (mMode) {
    case ModeWidget::Select:
        render(highlightOnlySelectedCurve, orderedCurves);
        break;
    case ModeWidget::Add:
        render(highlightOnlySelectedCurve, orderedCurves);
        break;
    case ModeWidget::Move:
        render(highlightOnlySelectedCurve, orderedCurves);
        break;
    case ModeWidget::Pan:
        render(false, orderedCurves);
        break;

    }
}

void BezierContourRenderer::setProjectionMatrix(QMatrix4x4 newMatrix)
{
    mProjectionMatrix = newMatrix;
}

void BezierContourRenderer::render(bool highlightOnlySelectedCurve, QVector<Curve*> curves)
{
    for(int i = 0; i < curves.size(); ++i)
    {
        Bezier* curve = dynamic_cast<Bezier*>(curves[i]);

        if(curve == nullptr)
            continue;

        QVector4D color = QVector4D(0,0,0,1);

        if(highlightOnlySelectedCurve && !curve->selected())
            color = QVector4D(0.9, 0.9, 0.9, 1);

        mShader->bind();

        QVector<QVector2D> controlPoints = curve->getControlPointPositions();

        // Control Points Buffer
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mControlPointsBuffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(QVector2D) * controlPoints.size(), controlPoints.constData());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Uniform Variables
        mShader->setUniformValue(mProjectionMatrixLocation, mProjectionMatrix);
        mShader->setUniformValue(mColorLocation, color);
        mShader->setUniformValue(mThicknessLocation, mZoomRatio * 4);
        mShader->setUniformValue(mTicksDeltaLocation, mTicksDelta);
        mShader->setUniformValue(mControlPointsCountLocation, controlPoints.size());

        QOpenGLVertexArrayObject::Binder binder(&mTicksVertexArray);
        glDrawArrays(GL_POINTS, 0, mTicks.size());
        mShader->release();
    }
}

QVector4D BezierContourRenderer::lighter(QVector4D color, float factor)
{
    float x = color.x() + factor * (1 - color.x());
    float y = color.y() + factor * (1 - color.y());
    float z = color.z() + factor * (1 - color.z());
    float w = color.w();


    if(1 < x)
        x = 1;
    if(1 < y)
        y = 1;
    if(1 < z)
        z = 1;

    return QVector4D(x,y,z,w);

}

QVector<Curve *> BezierContourRenderer::orderCurves(QVector<Curve *> curves)
{
    QVector<Curve*> orderedCurves;

    if(curves.size() == 0)
        return orderedCurves;

    orderedCurves << curves[0];

    for(int i = 1; i < curves.size(); i++)
    {
        Curve* curve = curves[i];
        if(orderedCurves.last()->z() <= curve->z())
        {
            orderedCurves << curve;
        }
        else
        {
            for(int j = 0; j < orderedCurves.size(); j++)
            {
                if(orderedCurves[j]->z() > curve->z())
                {
                    orderedCurves.insert(j, curve);
                    break;
                }
            }
        }
    }

    return orderedCurves;
}

float BezierContourRenderer::zoomRatio() const
{
    return mZoomRatio;
}

void BezierContourRenderer::setZoomRatio(float newZoomRatio)
{
    mZoomRatio = newZoomRatio;
}

ModeWidget::Mode BezierContourRenderer::mode() const
{
    return mMode;
}

void BezierContourRenderer::setMode(ModeWidget::Mode newMode)
{
    mMode = newMode;
}

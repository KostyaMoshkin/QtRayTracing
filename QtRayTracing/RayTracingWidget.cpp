#include "RayTracingWidget.h"
#include "QtRayTracing.h"

#include "KeyPress.h"

RayTracingWidget::RayTracingWidget(QWidget* pParent_)
	: QOpenGLWidget{ pParent_}
{
	m_pCamera = new Camera(45.0f, 0.01f, 100.0f);

	m_bKeyPress = new bool[120];
	for (int i = 0; i < 120; ++i)
		m_bKeyPress[i] = false;

    setFocusPolicy(Qt::ClickFocus);

    m_timer.start(1);
    connect(&m_timer, &QTimer::timeout, this, &RayTracingWidget::paintGL);
}

RayTracingWidget::~RayTracingWidget()
{
	delete m_pCamera;
	delete[] m_bKeyPress;
}

void RayTracingWidget::initialize()
{
    m_scene.setLightPosition(-2.0f, 2.0f, -2.0f);

    m_pCamera->onResize(size().width(), size().height());

    m_timePoint = std::chrono::steady_clock::now();

    static const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec2 posAttr;\n"
        "out vec2 TexCoord;\n"
        "void main() {\n"
        "   gl_Position = vec4(posAttr, 1.0, 1.0);\n"
        "   TexCoord = 1.0 - (posAttr + 1.0) / 2.0;\n"
        "}\n";

    static const char* fragmentShaderSource =
        "#version 330 core\n"
        "uniform sampler2D ourTexture;\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "   FragColor = texture(ourTexture, TexCoord);\n"
        "}\n";

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    Q_ASSERT(m_posAttr != -1);

    static const GLfloat vertices[] = {
         -1.0f,  -1.0f,
         -1.0f,  +1.0f,
         +1.0f,  +1.0f,
         +1.0f,  +1.0f,
         +1.0f,  -1.0f,
         -1.0f,  -1.0f
    };

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenTextures(1, &m_texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_program->release();
}

void RayTracingWidget::initializeGL()
{
	initializeOpenGLFunctions();
    initialize();
}

bool RayTracingWidget::updateTexture()
{
    if (m_texture == UINT_MAX)
        return false;

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::milliseconds elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_timePoint);
    m_timePoint = now;

    m_nMilliSecondsCount += elapsed_milliseconds.count();

    float fStep = (float)elapsed_milliseconds.count() / 300.0;

    if (m_nMilliSecondsCount >= 1000)
    {
        putStatusInfo(QString::number(m_nTimeTick));
        m_nTimeTick = 0;
        m_nMilliSecondsCount -= 1000;
    }
    else
        ++m_nTimeTick;

    float fMouseStep = 0.05;

    if (m_bKeyPress[(int)EKeyPress::key_left])
        m_pCamera->moveRight(fStep);
    if (m_bKeyPress[(int)EKeyPress::key_right])
        m_pCamera->moveLeft(fStep);
    if (m_bKeyPress[(int)EKeyPress::key_up])
        m_pCamera->moveDown(fStep);
    if (m_bKeyPress[(int)EKeyPress::key_down])
        m_pCamera->moveUp(fStep);
    if (m_nZoom != 0)
    {
        m_pCamera->zoom(m_nZoom * fMouseStep);
        m_nZoom = 0;
    }

    if (m_ptMouseMove != QPoint(0, 0))
    {
        m_pCamera->rotate((float)m_ptMouseMove.y() * fMouseStep, -(float)m_ptMouseMove.x() * fMouseStep);

        m_ptMouseMove = QPoint(0, 0);
    }

    if (!m_pCamera->isUpadated())
        return false;

    m_scene.process(m_pCamera);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)size().width(), (GLsizei)size().height(), GL_RGBA, GL_FLOAT, (void*)m_vTexture.data());

    return true;
}

void RayTracingWidget::paintGL()
{
    bool bUpdated = updateTexture();

    m_program->bind();

    glEnableVertexAttribArray(m_posAttr);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(m_posAttr);

    m_program->release();

    update();
}

void RayTracingWidget::resizeGL(int w_, int h_)
{
	glViewport(0, 0, w_, h_);

    m_vTexture.resize(w_ * h_);
    m_scene.setBuffer((float*)m_vTexture.data(), w_, h_);

    m_pCamera->onResize(w_, h_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_, h_, 0, GL_RGBA, GL_FLOAT, nullptr);
}

bool RayTracingWidget::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        return true;
    default:
        return QOpenGLWidget::event(event);
    }
}

void RayTracingWidget::keyPressEvent(QKeyEvent* event_)
{
    if (event_->key() == Qt::Key_Left)
        m_bKeyPress[(int)EKeyPress::key_left] = true;
    if (event_->key() == Qt::Key_Right)
        m_bKeyPress[(int)EKeyPress::key_right] = true;
    if (event_->key() == Qt::Key_Up)
        m_bKeyPress[(int)EKeyPress::key_up] = true;
    if (event_->key() == Qt::Key_Down)
        m_bKeyPress[(int)EKeyPress::key_down] = true;
}

void RayTracingWidget::keyReleaseEvent(QKeyEvent* event_)
{
    if (event_->key() == Qt::Key_Left)
        m_bKeyPress[(int)EKeyPress::key_left] = false;
    if (event_->key() == Qt::Key_Right)
        m_bKeyPress[(int)EKeyPress::key_right] = false;
    if (event_->key() == Qt::Key_Up)
        m_bKeyPress[(int)EKeyPress::key_up] = false;
    if (event_->key() == Qt::Key_Down)
        m_bKeyPress[(int)EKeyPress::key_down] = false;
}

void RayTracingWidget::mouseMoveEvent(QMouseEvent* event_)
{
    if (!m_bMousePress)
        return;

    QPoint ptMousePos = event_->globalPosition().toPoint();

    m_ptMouseMove += (m_ptMousePos - ptMousePos);
    m_ptMousePos = ptMousePos;
}

void RayTracingWidget::mousePressEvent(QMouseEvent* event_)
{
    if (event_->button() != Qt::LeftButton)
        return;

    m_bMousePress = true;

    m_ptMousePos = event_->globalPosition().toPoint();

    m_ptMouseMove = QPoint(0, 0);
}

void RayTracingWidget::mouseReleaseEvent(QMouseEvent* event_)
{
    m_bMousePress = false;
}

void RayTracingWidget::wheelEvent(QWheelEvent* event_)
{
    QOpenGLWidget::wheelEvent(event_);

    m_nZoom += event_->angleDelta().y() / 120;
}

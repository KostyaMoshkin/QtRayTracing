#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include <QMouseEvent>

#include <QTimer>

#include "Camera.h"
#include "Scene.h"

class RayTracingWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

	struct Color
	{
		float r;
		float g;
		float b;
		float w;
	};

private:
	Scene m_scene;
	Camera* m_pCamera = nullptr;

	bool* m_bKeyPress = nullptr;

	GLint m_posAttr = 0;

	QOpenGLShaderProgram* m_program = nullptr;
	float m_frame = 0;

	unsigned int m_texture = UINT_MAX;
	std::chrono::steady_clock::time_point m_timePoint;

	std::vector<Color> m_vTexture;

	bool m_bMousePress = false;

	QPoint m_ptMousePos;
	QPoint m_ptMouseMove;

	int m_nZoom = 0;

	QTimer m_timer;

	int m_nTimeTick = 0;
	int m_nMilliSecondsCount = 0;

public:
	RayTracingWidget(QWidget* pParent_ = nullptr);
	~RayTracingWidget();

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w_, int h_) override;

	bool event(QEvent* event) override;
	void keyPressEvent(QKeyEvent* event_) override;
	void keyReleaseEvent(QKeyEvent* event_) override;
	void mouseMoveEvent(QMouseEvent* event_) override;
	void mousePressEvent(QMouseEvent* event_) override;
	void mouseReleaseEvent(QMouseEvent* event_) override;
	void wheelEvent(QWheelEvent* event_) override;


protected:
	void initialize();
	bool updateTexture();

signals:
	void putStatusInfo(QString message_);

};


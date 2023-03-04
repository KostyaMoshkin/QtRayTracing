#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera(float fVerticlaFOV_, float fNearClip_, float fFarClip_)
	: m_fVerticlaFOV(fVerticlaFOV_), m_fNearClip(fNearClip_), m_fFarClip(fFarClip_)
{
	m_position = glm::vec3(0, -0.01, 3);
	m_forwardDirection = glm::vec3(0, 0, -1);
	m_upDirection = glm::vec3(0, 1, 0);
}

void Camera::onUpdate()
{
}

void Camera::onResize(uint32_t nWidth_, uint32_t nHeight_)
{
	m_nViewWidth = nWidth_;
	m_nViewHeight = nHeight_;

	recalculateProjection();
	recalculateView();
}

void Camera::moveLeft(float fStep_)
{
	glm::vec3 rightDirection = glm::normalize(glm::cross(m_forwardDirection, m_upDirection));
	m_position -= m_fSpeed * rightDirection * fStep_;

	recalculateView();
}

void Camera::moveRight(float fStep_)
{
	glm::vec3 rightDirection = glm::normalize(glm::cross(m_forwardDirection, m_upDirection));
	m_position += m_fSpeed * rightDirection * fStep_;

	recalculateView();
}

void Camera::moveUp(float fStep_)
{
	m_position += m_fSpeed * m_upDirection * fStep_;

	recalculateView();
}

void Camera::moveDown(float fStep_)
{
	m_position -= m_fSpeed * m_upDirection * fStep_;

	recalculateView();
}

void Camera::zoom(float fStep_)
{
	m_position += m_fSpeed * m_forwardDirection * fStep_ * 5.0f;

	recalculateView();
}

void Camera::rotate(float fPitch_, float fYaw_)
{
	glm::vec3 rightDirection = glm::cross(m_forwardDirection, m_upDirection);

	glm::quat quaternion = glm::normalize(glm::cross(
		glm::angleAxis(-fPitch_ * 0.05f, rightDirection),
		glm::angleAxis(-fYaw_ * 0.05f, m_upDirection)
	));

	m_forwardDirection = glm::rotate(quaternion, m_forwardDirection);

	recalculateView();
}

bool Camera::isUpadated()
{
	if (m_bUpdated)
	{
		m_bUpdated = false;
		return true;
	}

	return false;
}

void Camera::recalculateProjection()
{
	m_projection = glm::perspectiveFov(glm::radians(m_fVerticlaFOV), (float)m_nViewWidth, (float)m_nViewHeight, m_fNearClip, m_fFarClip);
	m_inverseProjection = glm::inverse(m_projection);

	m_bUpdated = true;
}

void Camera::recalculateView()
{
	m_view = glm::lookAt(m_position, m_position + m_forwardDirection, m_upDirection);
	m_inverseView = glm::inverse(m_view);

	m_bUpdated = true;
}


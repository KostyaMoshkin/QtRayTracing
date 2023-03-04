#pragma once
#include <GLM/glm.hpp>
#include <vector>

class Camera
{
public:
	Camera(float fVerticlaFOV_, float fNearClip_, float fFarClip_);

public:
	void onUpdate();
	void onResize(uint32_t nWidtg_, uint32_t nHeight_);

	const glm::mat4& getProjection() const { return m_projection; };
	const glm::mat4& getInverseProjection() const { return m_inverseProjection; };

	const glm::mat4& getView() const { return m_view; };
	const glm::mat4& getInverseView() const { return m_inverseView; };

	const glm::vec3& getPosition() const { return m_position; };
	const glm::vec3& getDirection() const { return m_forwardDirection; };

	std::vector<glm::vec3>& getRayDirection() { return m_vRayDirection; }
	std::vector<glm::vec4>& getRayDirection4() { return m_vRayDirection4; }

public:
	void moveLeft(float fStep_);
	void moveRight(float fStep_);
	void moveUp(float fStep_);
	void moveDown(float fStep_);
	void zoom(float fStep_);
	void rotate(float fPitch_, float fYaw_);

	bool isUpadated();

private:
	void recalculateProjection();
	void recalculateView();

private:
	glm::mat4 m_projection{ 1.0f };
	glm::mat4 m_view{ 1.0f };

	glm::mat4 m_inverseProjection{ 1.0f };
	glm::mat4 m_inverseView{ 1.0f };

	glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_forwardDirection{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_upDirection{ 0.0f, 1.0f, 0.0f };

	std::vector<glm::vec3> m_vRayDirection;
	std::vector<glm::vec4> m_vRayDirection4;

	float m_fVerticlaFOV = 45.0f;
	float m_fNearClip = 0.1f;
	float m_fFarClip = 100.0f;

	float m_fSpeed = 0.5f;

	bool m_bUpdated = true;

	uint32_t m_nViewWidth = 0;
	uint32_t m_nViewHeight = 0;
};


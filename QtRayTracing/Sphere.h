#pragma once

#include <GLM/glm.hpp>

struct Sphere
{
	glm::vec4 m_position;
	glm::vec4 m_color;
	float m_fRadius;
	float fBuf[3]; //  Reserved for OpenCL alignment
	int m_nMaterialIndex;
	int iBuff[3];  //  Reserved for OpenCL alignment

public:
	Sphere(glm::vec3 position_, float radius_, glm::vec3 color_, int nMaterialIndex_) 
		: m_position(position_, 0), m_fRadius(radius_), m_color(color_, 0), m_nMaterialIndex(nMaterialIndex_) {  }
};
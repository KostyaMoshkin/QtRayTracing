#pragma once

#include <GLM/glm.hpp>

struct Materials
{
public:
	glm::vec4 m_color;
	float m_fRoughness;
	float fBuf[3]; //  Reserved for OpenCL alignment

public:
	Materials(glm::vec4 color_, float fRoughness_) : m_color(color_), m_fRoughness(fRoughness_) {}
};

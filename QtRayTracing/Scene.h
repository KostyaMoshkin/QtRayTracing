#pragma once
#include "Sphere.h"

#include "Camera.h"

#include <fstream>
#include <streambuf>

#include <omp.h>

#include <vector>
#include <GLM/glm.hpp>

#include <CL/cl.hpp>

class Scene
{
	cl::Device m_device;
	cl::Kernel m_kernel;
	cl::Context m_context;
	cl::CommandQueue m_queue;

	cl::Buffer m_screenBuffer;
	cl::Buffer m_sphereBuffer;
	cl::Buffer m_rayOriginBuffer;
	cl::Buffer m_rayDirectionBuffer;
	cl::Buffer m_lightDirectionBuffer;
	cl::Buffer m_inverseProjectionBuffer;
	cl::Buffer m_inverseViewBuffer;

	std::string m_sErrorMessage;

	float* m_pBuffer = nullptr;
	int m_nWidth; 
	int m_nHeight;

	glm::vec3 m_lightPosition;

	std::vector<Sphere> m_vSphere;

public:
	Scene();
	~Scene();

public:
	void setBuffer(float* pBuffer_, int nWidth_, int nHeight_);
	void process(Camera* pCamera_);
	void setLightPosition(float x_, float y_, float z_);
};


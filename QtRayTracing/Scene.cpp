#include "Scene.h"
#include "Sphere.h"

Scene::Scene()
{
	float fBigR = 50000.0f;
	m_vSphere.push_back({ glm::vec3(0.0f, fBigR, 0.0f), fBigR, glm::vec3(0.3f, 0.3f, 0.3f), 0 });
	m_vSphere.push_back({ glm::vec3(0.0f, -0.5f, -2.0f), 0.5f, glm::vec3(1, 0, 0), 0 });
	m_vSphere.push_back({ glm::vec3(1.0f, -0.3f, -0.5f), 0.3f, glm::vec3(1, 0, 1), 0 });
	m_vSphere.push_back({ glm::vec3(-1.0f, -0.3f, -0.5f), 0.3f, glm::vec3(0.8, 0.1, 0.4), 0 });
	m_vSphere.push_back({ glm::vec3(1.5f, -0.4f, -1.0f), 0.4f, glm::vec3(0, 1, 0), 0 });
	m_vSphere.push_back({ glm::vec3(-0.5f, -0.3f, 1.0f), 0.3f, glm::vec3(0.5f, 0.5f, 0), 0 });
	m_vSphere.push_back({ glm::vec3(0.1f, -0.1f, 1.5f), 0.1f, glm::vec3(0.3f, 0.3f, 0.6f), 0 });
	m_vSphere.push_back({ glm::vec3(0.1f, -0.9f, 0.5f), 0.3f, glm::vec3(0.1f, 0.9f, 0.6f), 0 });

	std::vector<cl::Platform> vPlatform;
	cl::Platform::get(&vPlatform);

	if (vPlatform.size() == 0) {
		return;
	}

	auto platform = vPlatform.front();

	std::vector<cl::Device> vDevice;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &vDevice);

	if (vDevice.size() == 0) {
		return;
	}

	m_device = vDevice.front();

	m_context = cl::Context({ m_device });
	cl::Program::Sources sources;

	std::ifstream t("E:\\VisualStudioProjects\\QtRayTracing\\QtRayTracing\\perPixel.kernel");
	std::string kernel_code((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	sources.push_back({ kernel_code.c_str(),kernel_code.length() });

	cl::Program program(m_context, sources);
	if (program.build({ m_device }) != CL_SUCCESS) {
		m_sErrorMessage = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_device);
		return;
	}

	m_sphereBuffer = cl::Buffer(m_context, CL_MEM_READ_ONLY, sizeof(Sphere) * m_vSphere.size());
	m_rayOriginBuffer = cl::Buffer(m_context, CL_MEM_READ_ONLY, 4 * sizeof(float));
	m_lightDirectionBuffer = cl::Buffer(m_context, CL_MEM_READ_ONLY, 4 * sizeof(float));
	m_inverseProjectionBuffer = cl::Buffer(m_context, CL_MEM_READ_ONLY, sizeof(float) * 4 * 4);
	m_inverseViewBuffer = cl::Buffer(m_context, CL_MEM_READ_ONLY, sizeof(float) * 4 * 4);


	m_kernel = cl::Kernel(program, "perPixel_kernel");
	m_kernel.setArg(1, m_sphereBuffer);
	m_kernel.setArg(2, m_rayOriginBuffer);
	m_kernel.setArg(3, m_lightDirectionBuffer);
	m_kernel.setArg(4, m_inverseProjectionBuffer);
	m_kernel.setArg(5, m_inverseViewBuffer);

	int nFigureCount = m_vSphere.size();
	m_kernel.setArg(6, nFigureCount);

	m_queue = cl::CommandQueue(m_context, m_device);

	m_queue.enqueueWriteBuffer(m_sphereBuffer, CL_TRUE, 0, sizeof(Sphere) * m_vSphere.size(), m_vSphere.data());
}

Scene::~Scene()
{
}

void Scene::setBuffer(float* pBuffer_, int nWidth_, int nHeight_)
{
	m_pBuffer = pBuffer_;
	m_nWidth = nWidth_;
	m_nHeight = nHeight_;

	m_screenBuffer = cl::Buffer(m_context, CL_MEM_WRITE_ONLY, 4 * sizeof(float) * m_nWidth * m_nHeight);

	m_kernel.setArg(0, m_screenBuffer);
}

void Scene::process(Camera* pCamera_)
{
	m_queue.enqueueWriteBuffer(m_sphereBuffer, CL_TRUE, 0, sizeof(Sphere) * m_vSphere.size(), m_vSphere.data());

	glm::vec3 rayOrigin = pCamera_->getPosition();
	m_queue.enqueueWriteBuffer(m_rayOriginBuffer, CL_TRUE, 0, 3 * sizeof(float), &rayOrigin);

	glm::mat4 inverseProjection = pCamera_->getInverseProjection();
	m_queue.enqueueWriteBuffer(m_inverseProjectionBuffer, CL_TRUE, 0, sizeof(float) * 4 * 4, &inverseProjection[0][0]);

	glm::mat4 inverseView = pCamera_->getInverseView();
	m_queue.enqueueWriteBuffer(m_inverseViewBuffer, CL_TRUE, 0, sizeof(float) * 4 * 4, &inverseView[0][0]);

	m_queue.enqueueNDRangeKernel(m_kernel, cl::NullRange, cl::NDRange(m_nWidth, m_nHeight), cl::NullRange);
	m_queue.finish();

	m_queue.enqueueReadBuffer(m_screenBuffer, CL_TRUE, 0,
		4 * sizeof(float) * m_nWidth * m_nHeight,
		m_pBuffer);
}

void Scene::setLightPosition(float x_, float y_, float z_)
{
	m_lightPosition = glm::normalize(glm::vec3(x_, y_, z_));
	m_queue.enqueueWriteBuffer(m_lightDirectionBuffer, CL_TRUE, 0, 3 * sizeof(float), &m_lightPosition);
}


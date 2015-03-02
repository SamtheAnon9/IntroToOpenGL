#include "Tutorial6.h"
#include "glm\glm.hpp"
#include "glm\ext.hpp"
#include "gl_core_4_4.h"
#include "GLFW\glfw3.h"
#include "FlyCamera.h"
#include "Renderer.h"
#include "glm\glm.hpp"

int Tutorial6::Init()
{
	int baseInit = Application::Init();
	if (baseInit != 0)
		return baseInit;

	m_camera = new FlyCamera(m_debugBar);
	m_camera->SetPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	m_camera->SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));



	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);

	m_renderer = new Renderer(m_camera, m_debugBar);
	
	std::vector<std::string> textures;
	std::vector<std::string> normalMaps;
	std::vector<bool> texchannels;
	std::vector<bool> normchannels;

	textures.push_back("../data/Enemyelite/EnemyElite3_D.tga");
	textures.push_back("../data/Enemyelite/Alienrifle_D.png");
	normalMaps.push_back("../data/Enemyelite/EnemyElite_N.tga");
	normalMaps.push_back("../data/Enemyelite/Alienrifle_N.png");
	texchannels.push_back(false);
	texchannels.push_back(false);
	normchannels.push_back(false);
	normchannels.push_back(false);

	m_renderer->LoadFBX("../data/Enemyelite/EnemyElite.fbx", &textures, &normalMaps, &texchannels, &normchannels);

	m_timer = 0;

	return 0;
}

void Tutorial6::Update(float a_deltaTime)
{
	m_timer += a_deltaTime;
	m_camera->Update(a_deltaTime);
	m_renderer->UpdateAnimation(m_timer);
}

void Tutorial6::Draw()
{
	m_renderer->Draw();
}

int Tutorial6::Deinit()
{

	delete m_camera;

	m_renderer->CleanupBuffers();

	return Application::Deinit();
}
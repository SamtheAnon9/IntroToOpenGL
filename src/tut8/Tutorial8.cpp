#include "Tutorial8.h"
#include "glm\glm.hpp"
#include "glm\ext.hpp"
#include "gl_core_4_4.h"
#include "GLFW\glfw3.h"
#include "FlyCamera.h"
#include "Renderer.h"
#include "glm\glm.hpp"

int Tutorial8::Init()
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

	srand(time(0));

	particleDirection = true;

	//std::vector<std::string> textures;
	//std::vector<std::string> normalMaps;
	//std::vector<bool> texchannels;
	//std::vector<bool> normchannels;
	//
	//textures.push_back("../data/Enemyelite/EnemyElite3_D.tga");
	//textures.push_back("../data/Enemyelite/Alienrifle_D.png");
	//normalMaps.push_back("../data/Enemyelite/EnemyElite_N.tga");
	//normalMaps.push_back("../data/Enemyelite/Alienrifle_N.png");
	//texchannels.push_back(false);
	//texchannels.push_back(false);
	//normchannels.push_back(false);
	//normchannels.push_back(false);
	//
	//m_renderer->LoadFBX("../data/Enemyelite/EnemyElite.fbx", &textures, &normalMaps, &texchannels, &normchannels);

	//m_emitter = m_renderer->CreateEmitter(100000,			//Max particles
	//									  0.1f,				//Lifespan minimum 
	//									  100.0f,			//Lifespan maximum
	//									  5.0f,				//Velocity minimum
	//									  5.0f,				//Velocity maximum
	//									  1.0f,				//Start size
	//									  0.1f,				//End size
	//									  vec4(1, 0, 0, 1), //Start colour
	//									  vec4(1, 1, 0, 1), //End colour
	//									  true);			//GPU based
   
	//m_emitter = m_renderer->CreateEmitter(10000000,			//Max particles
	//									  0.1f,				//Lifespan minimum 
	//									  10.0f,			//Lifespan maximum
	//									  0.0f,				//Velocity minimum
	//									  0.1f,				//Velocity maximum
	//									  0.01f,			//Start size
	//									  0.01f,			//End size
	//									  vec4(0.3f, 0.3f, 0.3f, 1), //Start colour
	//									  vec4(0.2f, 0.2f, 0.2f, 1), //End colour
	//									  true);			//GPU based

	m_emitter = m_renderer->CreateEmitter(100000,			//Max particles
										  0.1f,				//Lifespan minimum 
										  10.0f,			//Lifespan maximum
										  0.0f,				//Velocity minimum
										  1.0f,				//Velocity maximum
										  1.0f,				//Start size
										  0.1f,				//End size
										  vec4(1, 0, 0, 1), //Start colour
										  vec4(1, 1, 0, 1), //End colour
										  true);			//GPU based

	m_emitter2 = m_renderer->CreateEmitter(100000,			 //Max particles
										   0.1f,			 //Lifespan minimum 
										   10.0f,			 //Lifespan maximum
										   0.0f,			 //Velocity minimum
										   1.0f,			 //Velocity maximum
										   1.0f,			 //Start size
										   0.1f,			 //End size
										   vec4(0, 0, 1, 1), //Start colour
										   vec4(0, 1, 1, 1), //End colour
										   true);			 //GPU based

	m_timer = 0;

	return 0;
}

void Tutorial8::Update(float a_deltaTime)
{
	m_timer += a_deltaTime;

	m_camera->Update(a_deltaTime);

	m_renderer->UpdateAnimation(m_timer);

	//m_renderer->SetEmitterPosition(m_emitter, true, m_renderer->GetEmitterPosition(m_emitter, true) + vec3((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX) * 2 - vec3(1, 1, 1));
	//m_renderer->SetEmitterPosition(m_emitter, true, m_renderer->GetEmitterPosition(m_emitter, true) + vec3(1, 0, 0));

	if (m_renderer->GetEmitterPosition(m_emitter, true).y > 40.0f)
		particleDirection = false;
	else if (m_renderer->GetEmitterPosition(m_emitter, true).y < -40.0f)
		particleDirection = true;

	m_renderer->SetEmitterPosition(m_emitter,  true, vec3(cosf(m_timer),				 m_renderer->GetEmitterPosition(m_emitter,  true).y + ((particleDirection) ? 0.1f : -0.1f), sinf(m_timer)));
	m_renderer->SetEmitterPosition(m_emitter2, true, vec3(cosf(m_timer + 3.1415926535f), m_renderer->GetEmitterPosition(m_emitter2, true).y + ((particleDirection) ? 0.1f : -0.1f), sinf(m_timer + 3.1415926535f)));
}

void Tutorial8::Draw()
{
	m_renderer->Draw();
}

int Tutorial8::Deinit()
{

	delete m_camera;

	m_renderer->CleanupBuffers();

	m_renderer->DestroyEmitter(m_emitter, true);


	return Application::Deinit();
}
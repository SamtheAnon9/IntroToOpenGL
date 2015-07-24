#include "PhysicsConcrete2.h"
#include "Renderer.h"
#include "WalkCamera.h"
#include "InputManager.h"
//Used for addlight function
#include "tut13\Tutorial13.h"

#include <iostream>

int PhysicsConcrete2::Init()
{
	int baseInit = PhysicsBase::Init();
	if (baseInit != 0)
		return baseInit;

	g_physicsMaterial = g_physics->createMaterial(0.5f, 0.5f, 0.5f);

	m_gun = m_renderer->LoadOBJ("../data/gun/crossbow.obj");
	m_renderer->LoadTexture("../data/gun/texture.jpg", m_gun);
	m_renderer->LoadAmbient("../data/gun/texture.jpg", m_gun);
	m_renderer->LoadSpecularMap("../data/gun/specular.jpg", m_gun);

	m_spawnTimer = 0.0f;
	m_shootTimer = 0.0f;
	m_shootForce = 100.0f;

	unsigned int proceduralPlane;
	float buffer;
	AddProceduralPlane(99, 9, 9, vec3(0, -300, 0), g_physicsMaterial, proceduralPlane, buffer, 100);
	m_renderer->LoadTexture("../data/checkerboard.png", proceduralPlane);
	m_renderer->LoadAmbient("../data/checkerboard.png", proceduralPlane);

	TwAddVarRW(m_debugBar, "Shoot Force", TW_TYPE_FLOAT, &m_shootForce, "");

	TwAddSeparator(m_debugBar, "Lights", "");
	TwAddButton(m_debugBar, "AddLight", AddLight, (void*)(m_renderer), "");

	//PxBoxGeometry playerBox(1, 2, 1);
	//PxTransform playerPos(PxVec3(0, 0, 0));
	//m_player = PxCreateDynamic(*g_physics, playerPos, playerBox, *g_physicsMaterial, 10);
	//m_player->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	//g_physicsScene->addActor(*m_player);

	m_walkSpeed = 1.0f;
	m_verticleSpeed = -10.0f;

	//Set up the player
	PxControllerManager* controllerManager = PxCreateControllerManager(*g_physicsScene);
	PxCapsuleControllerDesc desc;
	desc.contactOffset = 0.05f;
	desc.height = 3.0f;
	desc.material = g_physicsMaterial;
	desc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	desc.position = PxExtendedVec3(0, 1000, 0);
	desc.radius = 2.0f;
	desc.stepOffset = 0.1f;
	g_playerController = controllerManager->createController(desc);

	//Cloth stuff
	PxClothParticle* vertices = new PxClothParticle[10 * 10];
	for (unsigned int r = 0; r < 10; ++r)
	{
		for (unsigned int c = 0; c < 10; ++c)
		{
			vertices[r * 10 + c].pos = PxVec3((float)c - 10 / 2, 0, (float)r - 10 / 2);
			vertices[r * 10 + c].invWeight = 1.0f;
		}
	}
	vertices[0].invWeight = 0.0f;
	vertices[90].invWeight = 0.0f;

	PxU32* primitives = new PxU32[(10 - 1) * (10 - 1) * 4];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (10 - 1); ++r)
	{
		for (unsigned int c = 0; c < (10 - 1); ++c)
		{
			primitives[index++] = r * 10 + c;
			primitives[index++] = r * 10 + c + 1;
			primitives[index++] = (r + 1) * 10 + c;
			primitives[index++] = (r + 1) * 10 + c + 1;
		}
	}

	PxClothMeshDesc clothDesc;
	clothDesc.points.data = vertices;
	clothDesc.points.count = 10 * 10;
	clothDesc.points.stride = sizeof(PxClothParticle);
	clothDesc.invMasses.data = &vertices->invWeight;
	clothDesc.invMasses.count = 10 * 10;
	clothDesc.invMasses.stride = sizeof(PxClothParticle);
	clothDesc.quads.data = primitives;
	clothDesc.quads.count = (10 - 1) * (10 - 1);
	clothDesc.quads.stride = sizeof(PxU32) * 4;
	
	PxClothFabric* fabric = PxClothFabricCreate(*g_physics, clothDesc, PxVec3(0, -1, 0));
	PxTransform clothPose = PxTransform(PxVec3(0.0f, 0.0f, 0.0f));
	m_cloth = g_physics->createCloth(clothPose, *fabric, vertices, PxClothFlags());
	m_cloth->setClothFlag(PxClothFlag::eSCENE_COLLISION, true);
	m_cloth->setSolverFrequency(240.0f);
	g_physicsScene->addActor(*m_cloth);

	delete[] vertices;
	delete[] primitives;

	m_clothGrid = m_renderer->GenerateGrid(9, 9);

	return 0;
}

int PhysicsConcrete2::Deinit()
{
	return PhysicsBase::Deinit();
}

void PhysicsConcrete2::Update(float a_deltaTime)
{
	PhysicsBase::Update(a_deltaTime);

#pragma region Physics Spawning
	m_spawnTimer += a_deltaTime;
	if (m_spawnTimer >= 1.0f)
	{
		std::cout << m_models.size() << std::endl;

		vec3 randPos = vec3(((float)rand() / (float)RAND_MAX) * 20.0f, ((float)rand() / (float)RAND_MAX) * 20.0f + 200.0f, ((float)rand() / (float)RAND_MAX) * 20.0f);

		if (rand() % 2 == 0)
			AddBox(g_physicsMaterial, 10.0f, vec3(2.0f, 2.0f, 2.0f), randPos, true);
		else
			AddSphere(g_physicsMaterial, 10.0f, 2.0f, randPos, true);

		m_lights.push_back(m_renderer->CreatePointLight(vec3(((float)rand() / (float)RAND_MAX), ((float)rand() / (float)RAND_MAX), ((float)rand() / (float)RAND_MAX)), 20, false, randPos));

		m_spawnTimer = 0;
	}
#pragma endregion

#pragma region UpdateLights
	for (unsigned int i = 0; i < m_lights.size(); ++i)
	{
		if (m_lights[i] == -1)
			continue;

		PxRigidActor* actor = g_physicsActors[i];
		if (actor->isRigidDynamic())
		{
			if (((PxRigidDynamic*)actor)->isSleeping())
				continue;
		}

		PxU32 shapesIndex = actor->getNbShapes();
		PxShape** shapes = new PxShape*[shapesIndex];

		actor->getShapes(shapes, shapesIndex);
		--shapesIndex;

		PxMat44 m(PxShapeExt::getGlobalPose(*shapes[shapesIndex], *actor));
		glm::mat4 transform(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
			m.column1.x, m.column1.y, m.column1.z, m.column1.w,
			m.column2.x, m.column2.y, m.column2.z, m.column2.w,
			m.column3.x, m.column3.y, m.column3.z, m.column3.w);

		m_renderer->SetLightPosition(m_lights[i], vec3(transform[3]));

		delete[] shapes;
	}
#pragma endregion

#pragma region Player Movement
	glm::mat4 cameraWorld = m_camera->GetWorldTransform();
	//m_player->setKinematicTarget(PxTransform(PxVec3(cameraWorld[3].x, cameraWorld[3].y, cameraWorld[3].z)));
	PxVec3 displacement = PxVec3(0, 0, 0);
	bool notZero = false;
	if (InputManager::GetKey(Keys::W))
	{
		displacement -= PxVec3(cameraWorld[2].x, (m_canFly ? cameraWorld[2].y : 0), cameraWorld[2].z);
		notZero = true;
	}
	if (InputManager::GetKey(Keys::A))
	{
		displacement -= PxVec3(cameraWorld[0].x, (m_canFly ? cameraWorld[0].y : 0), cameraWorld[0].z);
		notZero = true;
	}
	if (InputManager::GetKey(Keys::S))
	{
		displacement += PxVec3(cameraWorld[2].x, (m_canFly ? cameraWorld[2].y : 0), cameraWorld[2].z);
		notZero = true;
	}
	if (InputManager::GetKey(Keys::D))
	{
		displacement += PxVec3(cameraWorld[0].x, (m_canFly ? cameraWorld[0].y : 0), cameraWorld[0].z);
		notZero = true;
	}
	if (notZero)
		displacement = displacement.getNormalized() * m_walkSpeed;

	if (InputManager::GetKey(Keys::SPACE))
	{
		PxControllerState state;
		g_playerController->getState(state);
		if (state.collisionFlags > 0 || m_canFly)
			m_verticleSpeed = 0.5f;
	}

	if (m_verticleSpeed > -10.0f && !m_canFly || m_verticleSpeed > 0 && m_canFly)
		m_verticleSpeed -= a_deltaTime;

	displacement.y += m_verticleSpeed;

	PxControllerFilters filters;
	g_playerController->move(displacement, 0.01f, a_deltaTime, filters);

	PxExtendedVec3 playerPos = g_playerController->getPosition();
	PxExtendedVec3 footPos = g_playerController->getFootPosition();
	//I do these calculations individually inside this vector constructor because PxEtendedVec3 doesn't contain some of the necessary operators to do this.
	vec3 endPos = vec3(2.0f * playerPos.x - footPos.x, 2.0f * playerPos.y - footPos.y, 2.0f * playerPos.z - footPos.z);
	m_camera->SetPosition(endPos);
#pragma endregion

#pragma region Shooting
	m_shootTimer -= a_deltaTime;
	//Shooting and moving the gun should happen after updating the camera.
	m_renderer->SetTransform(glm::translate(glm::scale(glm::rotate(cameraWorld, 0.2f, vec3(1, 0, 0)), vec3(0.005f, 0.005f, 0.005f)), vec3(0.0f, -25.0f, -5.0f)), m_gun);
	if (m_shootTimer > 0.0f)
	{
		//Move the gun back if reloading.
		m_renderer->SetTransform(glm::translate(m_renderer->GetTransform(m_gun), vec3(0, 0, 5 * m_shootTimer)), m_gun);
	}

	if (InputManager::GetMouseDown(0) && m_shootTimer < 0.0f)
	{
		//Shoot
		m_shootTimer = 0.5f;
		AddSphere(g_physicsMaterial, 10.0f, 2.0f, vec3(cameraWorld[3]) + vec3(cameraWorld[2]) * -5.0f, true);
		vec3 forward = glm::rotateY(vec3(cameraWorld[2]), -0.2f);
		((PxRigidDynamic*)(g_physicsActors[g_physicsActors.size() - 1]))->setLinearVelocity(PxVec3(forward.x, forward.y, forward.z) * -1 * m_shootForce);

		m_lights.push_back(m_renderer->CreatePointLight(vec3(((float)rand() / (float)RAND_MAX), ((float)rand() / (float)RAND_MAX), ((float)rand() / (float)RAND_MAX)), 20, false, vec3(cameraWorld[3])));
	}
#pragma endregion

	PxClothParticleData* data = m_cloth->lockParticleData();
	std::vector<vec3> particlePositions;
	for (unsigned int i = 0; i < m_cloth->getNbParticles(); ++i)
	{
		PxVec3 particlePos = data->particles[i].pos;
		particlePositions.push_back(vec3(particlePos.x, particlePos.y, particlePos.z));
	}
	m_renderer->ModifyMesh(m_clothGrid, particlePositions);
	data->unlock();
}

void PhysicsConcrete2::Draw()
{
	PhysicsBase::Draw();
}
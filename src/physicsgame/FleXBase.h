#ifndef FLEX_BASE_H
#define FLEX_BASE_H

#include "Application.h"

#include <cassert>
#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <numeric>

#include "glm\glm.hpp"
#include "glm\ext.hpp"



class FlyCamera;
class Renderer;
struct FlexSolver;
struct FlexExtAsset;

using glm::vec4;
using glm::vec3;
using glm::quat;

class FleXBase : public Application
{
protected:
	virtual int Init() override;
	virtual int Deinit() override;
	//Update contains the FleXFence stuff required for async data transfers.
	//As such, any functions modifying/accessing flex should happen BEFORE update in deriving classes,
	//then any functions that use that data (eg: updating render data) should happen AFTER update in deriving classes.
	virtual void Update(float a_deltaTime) override;
	virtual void Draw() override;

	//The camera used within the scene.
	FlyCamera* m_camera;
	//The renderer used to render the scene.
	Renderer* m_renderer;

	FlexSolver* g_solver;

	//TODO: Modify add cloth to provide more control over the cloth that is spawned (position, rotation, etc.)
	//Adds a cloth with the specified mesh dimensions. a_tetherIndices specifies which particles should be used as tethers (not use physics).
	void AddCloth(unsigned int a_dimensions, unsigned int a_numberOfTethers, unsigned int* a_tetherIndices, float a_height);
	void AddBox(vec3 a_position, quat a_rotation);
	void AddShape();

	float m_particleRadius;

	//Rendering and physics handles
	std::vector<unsigned int> m_clothModels;
	std::vector<unsigned int> m_boxModels;
	std::vector<FlexExtAsset*> g_cloths;
	std::vector<FlexExtAsset*> g_cubes;

	//Static shape variables
	//std::vector<FlexCollisionGeometry> g_shapeGeometry;
	//std::vector<vec4> m_shapePositions;
	//std::vector<quat> m_shapeRotations;
	//std::vector<uint32_t> m_shapeStarts;
	//std::vector<int> m_shapeFlags;

	//Rigidbody variables.
	std::vector<int> m_rigidOffsets;
	std::vector<int> m_rigidIndices;
	std::vector<float> m_rigidRestPositions;
	std::vector<float> m_rigidCoefficients;
	std::vector<vec3> m_rigidPositions;
	std::vector<quat> m_rigidRotations;

	//Cloth/spring variables
	//The triangle indices used within cloth within the game.
	std::vector<int> m_clothIndices;
	//Index to which particle within the m_particles array represents the first particle for each piece of cloth.
	std::vector<int> m_clothParticleStartIndices;
	std::vector<int> m_springIndices;
	std::vector<float> m_springRestLengths;
	std::vector<float> m_springStiffness;

	int m_numberOfParticles;
	int m_numberOfClothParticles;
	int m_numberOfActiveParticles;

	unsigned int m_currentHighestPhase;

	float* m_particles;
	float* m_velocities;
	int* m_phases;

	int* m_activeParticles;


	// calculates local space positions given a set of particles and rigid indices
	// Copy + pasted from FleX demo code and modified to use glm vectors, return normals, and to use indices that don't include cloth indices.
	void CalculateRigidOffsets(const vec4* restPositions, const int* offsets, const int* indices, int numRigids, vec3* localPositions, vec4* normals);
};


#endif
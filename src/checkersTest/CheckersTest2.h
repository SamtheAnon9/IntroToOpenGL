#ifndef CHECKERS_TEST_2_H
#define CHECKERS_TEST_2_H

#include "physX\PhysicsBase.h"

#include <thread>

class CheckersTest2 : public PhysicsBase
{
public:
	inline float const GetAmplitude()
	{
		return m_amplitude;
	}
	inline unsigned int const GetSeed()
	{
		return m_seed;
	}
	inline float const GetPersistence()
	{
		return m_persistence;
	}
	inline Renderer* const GetRenderer()
	{
		return m_renderer;
	}
	inline unsigned int const GetProceduralPlane()
	{
		return m_proceduralPlane;
	}
	inline PxMaterial* GetPhysicsMaterial()
	{
		return g_physicsMaterial;
	}
	inline void SetProceduralPlane(unsigned int a_object)
	{
		m_proceduralPlane = a_object;
	}
	//See PhysicsBase.h for an explanation of these arguments.
	inline unsigned int GenerateProceduralPlane(unsigned int a_dimensions, unsigned int a_noiseMapDimensions,
		   unsigned int a_stretch, const vec3& a_position, PxMaterial* a_material,
		   float a_amplitude, unsigned int a_seed = rand(), unsigned int a_octaves = 6, float a_persistence = 0.3f)
	{
		return AddProceduralPlane(a_dimensions, a_noiseMapDimensions, a_stretch, a_position, a_material, a_amplitude, a_seed, a_octaves, a_persistence);
	}

private:
	int Init();
	int Deinit();
	void Update(float a_deltaTime);
	void Draw();

	PxMaterial* g_physicsMaterial;
	PxController* g_playerController;

	float m_spawnTimer;

	float m_walkSpeed;

	float m_shootTimer;
	float m_shootForce;

	unsigned int m_gun;

	std::vector<unsigned int> m_physicsLights;

	//PxRigidDynamic* m_player;


	float m_amplitude;
	float m_persistence;
	unsigned int m_seed;
	unsigned int m_proceduralPlane;

#pragma region Checkers Member Variables and Methods
	void CheckersUpdate(float a_deltaTime);

	void HandleEnter(int(&a_board)[8][8], const unsigned int a_xPos, const unsigned int a_yPos, unsigned int &a_prevX, unsigned int &a_prevY, bool &a_turn, const bool a_changeEmitters, unsigned int &a_pieceSelected);
	bool ValidMove(const int a_board[8][8], const unsigned int a_xPos, const unsigned int a_yPos, const unsigned int a_prevX, const unsigned int a_prevY, const bool a_turn, const bool a_changeEmitters);

	void UseAIMove();
	void AIMove(int(&a_board)[8][8], const bool a_turn, const unsigned int a_difficulty);
	//As vectors of arrays don't work, a vector<vector<int>>> has been used to replace a vector<int[8][8]>
	std::vector<std::vector<std::vector<int>>> GetPossibleMoves(const int a_board[8][8], const bool a_turn);
	//Plays random moves for both sides until the game ends.
	int PlayUntilEnd(std::vector<std::vector<int>> a_board, const bool a_turn);


	std::vector<unsigned int> m_emitters;
	std::vector<unsigned int> m_pieceLights;

	const float M_TILE_WIDTH = 12.5f;

	//The int at each location on the board represents which emitter is at that location.
	int m_board[8][8];

	unsigned int m_currentX;
	unsigned int m_currentY;

	unsigned int m_previousX;
	unsigned int m_previousY;

	unsigned int m_positionMarker;
	unsigned int m_positionLight;
	unsigned int m_positionLight2;

	float m_colourTimer;

	float m_inputTimer;

	unsigned int m_pieceSelected;

	unsigned int m_aiDifficulty;

	std::thread m_aiThread;

	bool m_threadFinished;

	bool m_turn;
#pragma endregion
	

	//Variables for generating the random board.
	//float m_amplitude;
	//float m_persistence;
	//unsigned int m_seed;
};

#endif
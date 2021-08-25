#include "AsteroidsGame.h"

#include <chrono>
#include <algorithm>
#include <cmath>

namespace AsteroidsCPP
{
	Game::Game(float shipControlSpeed, float shipControlRotationSpeed, std::uint32_t asteroidTemplatesCount, std::uint32_t maxAsteroidsCount, const Vec2& viewportSize)
		: m_ShipControlSpeed(shipControlSpeed)
		, m_ShipControlRotationSpeed(shipControlRotationSpeed)
		, m_AsteroidTemplatesCount(asteroidTemplatesCount)
		, m_MaxAsteroidsCount(maxAsteroidsCount)
		, m_ViewportSize(viewportSize)
		, m_AsteroidsPositions(nullptr)
		, m_AsteroidsSpeeds(nullptr)
	{
		m_AsteroidsCount = 0;
		for (uint32_t i = 0; i < m_AsteroidTemplatesCount; i++)
			// We have twice the number of asteroids within each asteroids's types
			m_AsteroidsCount += m_MaxAsteroidsCount * (std::uint32_t)pow(2, i);

		m_AsteroidsPositions = new Vec2[m_AsteroidsCount];
		m_AsteroidsSpeeds = new Vec2[m_AsteroidsCount];

		srand(454846188);
		for (uint32_t i = 0; i < m_AsteroidsCount; i++)
		{
			m_AsteroidsPositions[i].x = (float)rand() / (float)RAND_MAX * 10.0f - 5.0f;
			m_AsteroidsPositions[i].y = (float)rand() / (float)RAND_MAX * 10.0f - 5.0f;

			m_AsteroidsSpeeds[i].x = (float)rand()/(float)RAND_MAX * 10.0f - 5.0f;
			m_AsteroidsSpeeds[i].y = (float)rand() / (float)RAND_MAX * 10.0f - 5.0f;
		}
	}

	Game::~Game()
	{
		delete m_AsteroidsPositions;
		delete m_AsteroidsSpeeds;
	}

	void Game::Update(KeyState keyState, float deltaTime)
	{
		ApplyShipControl(keyState, deltaTime);

		// Update positions and loop it
		for (uint32_t i = 0; i < m_AsteroidsCount; i++)
		{
			m_AsteroidsPositions[i] += m_AsteroidsSpeeds[i] * deltaTime;

			if (m_AsteroidsPositions[i].x > m_ViewportSize.x) m_AsteroidsPositions[i].x = -m_ViewportSize.x;
			if (m_AsteroidsPositions[i].x < -m_ViewportSize.x) m_AsteroidsPositions[i].x = m_ViewportSize.x;
			if (m_AsteroidsPositions[i].y > m_ViewportSize.y) m_AsteroidsPositions[i].y = -m_ViewportSize.y;
			if (m_AsteroidsPositions[i].y < -m_ViewportSize.y) m_AsteroidsPositions[i].y = m_ViewportSize.y;
		}

		// Check if the ship was destroyed

		// Check if projectiles are hitting asteroids
	}

	void Game::ApplyShipControl(KeyState keyState, float deltaTime)
	{
		float rotControl = keyState.Pressed(KeyState::Keys::Left) ? 1.0f : keyState.Pressed(KeyState::Keys::Right) ? -1.0f : 0.0f;
		m_ShipPosRot.z += rotControl * m_ShipControlRotationSpeed * deltaTime;

		// Make sure we stay between -360 -> 360
		while (m_ShipPosRot.z >= 360.0f)
			m_ShipPosRot.z -= 360.0f;
		while (m_ShipPosRot.z <= -360.0f)
			m_ShipPosRot.z += 360.0f;
	}
}

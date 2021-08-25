#include "AsteroidsGame.h"

#include <chrono>
#include <algorithm>
#include <cmath>

namespace AsteroidsCPP
{
	Game::Game(std::uint32_t asteroidTemplatesCount, std::uint32_t maxAsteroidsCount, const Vec2& viewportSize)
		: m_AsteroidTemplatesCount(asteroidTemplatesCount)
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

		m_ShipPos.x = 0.5f;
		m_ShipPos.y = 0.5f;
	}

	Game::~Game()
	{
		delete m_AsteroidsPositions;
		delete m_AsteroidsSpeeds;
	}

	void Game::Update(KeyState keyState, float deltaTime)
	{
		// Update positions and loop it
		m_ShipPos.x += deltaTime * 0.05f;
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
}

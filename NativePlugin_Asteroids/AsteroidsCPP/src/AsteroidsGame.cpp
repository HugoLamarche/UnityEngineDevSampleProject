#include "AsteroidsGame.h"

#include <chrono>
#include <algorithm>
#include <cmath>

// TODO: use #define _USE_MATH_DEFINES instead but mess with precompiled headers
#define M_PI 3.14159265358979323846f

namespace AsteroidsCPP
{
	Game::Game(float shipControlSpeed,
			   float shipControlRotationSpeed,
			   float shipMaxSpeed,
			   std::uint32_t asteroidTemplatesCount,
			   std::uint32_t maxAsteroidsCount,
			   float minAsteroidsSpeed,
			   float maxAsteroidsSpeed,
			   const Vec2& viewportSize)
		: m_ShipControlSpeed(shipControlSpeed)
		, m_ShipControlRotationSpeed(shipControlRotationSpeed)
		, m_ShipMaxSpeed(shipMaxSpeed)
		, m_AsteroidTemplatesCount(asteroidTemplatesCount)
		, m_MaxAsteroidsCount(maxAsteroidsCount)
		, m_MinAsteroidsSpeed(minAsteroidsSpeed)
		, m_MaxAsteroidsSpeed(maxAsteroidsSpeed)
		, m_ViewportSize(viewportSize)
		, m_AsteroidsPositions(nullptr)
		, m_AsteroidsSpeeds(nullptr)
		, m_ShipRot(0.0f)
	{
		m_AsteroidsCount = 0;
		for (uint32_t i = 0; i < m_AsteroidTemplatesCount; i++)
			// We have twice the number of asteroids within each asteroids's types
			m_AsteroidsCount += m_MaxAsteroidsCount * (std::uint32_t)pow(2, i);

		m_AsteroidsPositions = new Vec2[m_AsteroidsCount];
		m_AsteroidsSpeeds = new Vec2[m_AsteroidsCount];

		// TODO: Implement Wave system !
		srand(time(nullptr));

		// Only activate the first level of asteroids
		for (uint32_t i = 0; i < m_AsteroidsCount; i++)
		{
			if (i < m_MaxAsteroidsCount)
			{
				// TODO: Implement a better way to choose between one of the four sides
				// Create two value to represent the random start position on X or Y axis
				const float randX = (float)rand() / (float)RAND_MAX * viewportSize.x * 2.0f - viewportSize.x;
				const float randY = (float)rand() / (float)RAND_MAX * viewportSize.y * 2.0f - viewportSize.y;

				// Create the 4 combinaison possible
				const Vec2 startLines[4] = { Vec2(randX, viewportSize.y),
											 Vec2(viewportSize.x, randY),
											 Vec2(randX, -viewportSize.y),
											 Vec2(-viewportSize.x, randY)};

				// Choose between one of theses
				m_AsteroidsPositions[i] = startLines[rand()%4];

				m_AsteroidsSpeeds[i].x = (float)rand() / (float)RAND_MAX * 2.0f * ((m_MaxAsteroidsSpeed - m_MinAsteroidsSpeed) + m_MinAsteroidsSpeed) - m_MaxAsteroidsSpeed;
				m_AsteroidsSpeeds[i].y = (float)rand() / (float)RAND_MAX * 2.0f * ((m_MaxAsteroidsSpeed - m_MinAsteroidsSpeed) + m_MinAsteroidsSpeed) - m_MaxAsteroidsSpeed;
			}
			else
				m_AsteroidsPositions[i].x = NAN;
		}
	}

	Game::~Game()
	{
		delete m_AsteroidsPositions;
		delete m_AsteroidsSpeeds;
	}

	void Game::Update(KeyState keyState, float deltaTime)
	{
		UpdateAsteroids(deltaTime);
		ApplyShipControl(keyState, deltaTime);


		// Check if the ship was destroyed

		// Check if projectiles are hitting asteroids
	}

	void Game::UpdateAsteroids(float deltaTime)
	{
		for (uint32_t i = 0; i < m_AsteroidsCount; i++)
		{
			if (isnan(m_AsteroidsPositions[i].x))
				continue;

			m_AsteroidsPositions[i] += m_AsteroidsSpeeds[i] * deltaTime;
			LoopPosition(m_AsteroidsPositions[i]);
		}
	}

	void Game::ApplyShipControl(KeyState keyState, float deltaTime)
	{
		// Rotation Control
		float rotControl = keyState.Pressed(KeyState::Keys::Left) ? 1.0f : keyState.Pressed(KeyState::Keys::Right) ? -1.0f : 0.0f;
		m_ShipRot += rotControl * m_ShipControlRotationSpeed * deltaTime;

		// Make sure we stay between -360 -> 360
		while (m_ShipRot >= 360.0f)
			m_ShipRot -= 360.0f;
		while (m_ShipRot <= -360.0f)
			m_ShipRot += 360.0f;

		// Forward Control
		float forwardControl = keyState.Pressed(KeyState::Keys::Up) ? 1.0f : keyState.Pressed(KeyState::Keys::Down) ? -1.0f : 0.0f;
		Vec2 direction = Vec2(cos(m_ShipRot * M_PI / 180.0f), sin(m_ShipRot * M_PI / 180.0f)) * forwardControl;
		m_ShipSpeed += direction * m_ShipControlSpeed * deltaTime;

		// Constraint speed
		m_ShipSpeed.x = std::max(-m_ShipMaxSpeed, std::min(m_ShipMaxSpeed, m_ShipSpeed.x));
		m_ShipSpeed.y = std::max(-m_ShipMaxSpeed, std::min(m_ShipMaxSpeed, m_ShipSpeed.y));

		// Update Position
		m_ShipPos += m_ShipSpeed * deltaTime;
		LoopPosition(m_ShipPos);
	}

	void Game::LoopPosition(Vec2& position) const
	{
		if (position.x > m_ViewportSize.x) position.x = -m_ViewportSize.x;
		if (position.x < -m_ViewportSize.x) position.x = m_ViewportSize.x;
		if (position.y > m_ViewportSize.y) position.y = -m_ViewportSize.y;
		if (position.y < -m_ViewportSize.y) position.y = m_ViewportSize.y;
	}
}

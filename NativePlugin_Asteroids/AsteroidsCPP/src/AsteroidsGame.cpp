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
			   float shipSqrRadius,
			   std::uint32_t asteroidTemplatesCount,
			   std::uint32_t maxAsteroidsCount,
			   float minAsteroidsSpeed,
			   float maxAsteroidsSpeed,
			   float fireRate,
			   float bulletLifeSpan,
			   float bulletSpeed,
			   float bulletSqrRadius,
			   const Vec2& viewportSize)
		: m_ShipControlSpeed(shipControlSpeed)
		, m_ShipControlRotationSpeed(shipControlRotationSpeed)
		, m_ShipMaxSpeed(shipMaxSpeed)
		, m_ShipSqrRadius(shipSqrRadius)
		, m_AsteroidTemplatesCount(asteroidTemplatesCount)
		, m_MaxAsteroidsCount(maxAsteroidsCount)
		, m_MinAsteroidsSpeed(minAsteroidsSpeed)
		, m_MaxAsteroidsSpeed(maxAsteroidsSpeed)
		, m_FireRate(fireRate)
		, m_BulletLifeSpan(bulletLifeSpan)
		, m_BulletSpeed(bulletSpeed)
		, m_BulletSqrRadius(bulletSqrRadius)
		, m_ViewportSize(viewportSize)
		, m_TimeSinceLastFire(FLT_MAX)
		, m_NextBulletIndex(0)
		, m_ShipRot(0.0f)
		, m_ShipDestroyed(false)
	{
		m_AsteroidsTemplateSqrRadius = new float[m_AsteroidTemplatesCount];
		m_LevelsBoundaries = new std::uint32_t[m_AsteroidTemplatesCount];

		m_AsteroidsCount = 0;
		for (uint32_t i = 0; i < m_AsteroidTemplatesCount; i++)
		{
			// We have twice the number of asteroids within each asteroids's types
			m_AsteroidsCount += m_MaxAsteroidsCount * (std::uint32_t)pow(2, i);

			// Store the last index of each template type in the Asteroids array
			m_LevelsBoundaries[i] = m_AsteroidsCount - 1;

			// Radius are set using SetAsteroidTemplateRadius
			m_AsteroidsTemplateSqrRadius[i] = 0.0f;
		}

		m_AsteroidsPositions = new Vec2[m_AsteroidsCount];
		m_AsteroidsSpeeds = new Vec2[m_AsteroidsCount];

		// TODO: Implement Wave system !
		srand(time(nullptr));

		// Only activate the first level of asteroids
		for (uint32_t i = 0; i < m_AsteroidsCount; i++)
		{
			if (true)//i < m_MaxAsteroidsCount)
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

		// Make sure we have enough bullet
		m_BulletsCount = m_FireRate * m_BulletLifeSpan;
		m_BulletsPositions = new Vec2[m_BulletsCount];
		m_BulletsSpeed = new Vec2[m_BulletsCount];
		m_BulletsLife = new float[m_BulletsCount];

		for (uint32_t i = 0; i < m_BulletsCount; i++)
		{
			m_BulletsPositions[i].x = NAN;
		}
	}

	Game::~Game()
	{
		delete[] m_AsteroidsTemplateSqrRadius;
		delete[] m_LevelsBoundaries;
		delete[] m_AsteroidsPositions;
		delete[] m_AsteroidsSpeeds;
		delete[] m_BulletsPositions;
		delete[] m_BulletsSpeed;
		delete[] m_BulletsLife;
	}

	void Game::SetAsteroidTemplateSqrRadius(std::uint32_t level, float sqrRadius)
	{
		if (level < m_AsteroidTemplatesCount)
			m_AsteroidsTemplateSqrRadius[level] = sqrRadius;
	}

	std::uint32_t Game::GetLevelFromIndex(std::uint32_t index) const
	{
		std::uint32_t level = 0;
		while (index > m_LevelsBoundaries[level])
		{
			level++;
		}
		return level;
	}

	void Game::Update(KeyState keyState, float deltaTime)
	{
		if (m_ShipDestroyed)
			return;

		ApplyShipControl(keyState, deltaTime);
		UpdateAsteroids(deltaTime);
		UpdateBullets(deltaTime);
	}

	float SqrDistance(Vec2 a, Vec2 b)
	{
		return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
	}

	void Game::UpdateAsteroids(float deltaTime)
	{
		// TODO: Optimize by only iterate on active Asteroids (don't even loop)
		for (uint32_t i = 0; i < m_AsteroidsCount; i++)
		{
			if (isnan(m_AsteroidsPositions[i].x))
				continue;

			m_AsteroidsPositions[i] += m_AsteroidsSpeeds[i] * deltaTime;
			LoopPosition(m_AsteroidsPositions[i]);

			// Check if the ship was destroyed
			if (SqrDistance(m_AsteroidsPositions[i], m_ShipPos) < (m_ShipSqrRadius + m_AsteroidsTemplateSqrRadius[GetLevelFromIndex(i)]))
			{
				m_ShipDestroyed = true;
				break;
			}
		}
	}

	void Game::UpdateBullets(float deltaTime)
	{
		// TODO: Optimize and factorise with UpdateAsteroids
		for (uint32_t i = 0; i < m_BulletsCount; i++)
		{
			if (isnan(m_BulletsPositions[i].x))
				continue;

			m_BulletsLife[i] -= deltaTime;
			if (m_BulletsLife[i] <= 0.0f)
			{
				m_BulletsPositions[i].x = NAN;
				continue;
			}

			m_BulletsPositions[i] += m_BulletsSpeed[i] * deltaTime;
			LoopPosition(m_BulletsPositions[i]);

			// Check if we touch something
			for (uint32_t j = 0; j < m_AsteroidsCount; j++)
			{
				if (SqrDistance(m_BulletsPositions[i], m_AsteroidsPositions[j]) < (m_BulletSqrRadius + m_AsteroidsTemplateSqrRadius[GetLevelFromIndex(j)]))
				{
					m_BulletsPositions[i].x = NAN;
					m_AsteroidsPositions[j].x = NAN;
					break;
				}
			}
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
		const Vec2 direction = Vec2(cos(m_ShipRot * M_PI / 180.0f), sin(m_ShipRot * M_PI / 180.0f));
		m_ShipSpeed += direction * m_ShipControlSpeed * forwardControl * deltaTime;

		// Constraint Speed
		m_ShipSpeed.x = std::max(-m_ShipMaxSpeed, std::min(m_ShipMaxSpeed, m_ShipSpeed.x));
		m_ShipSpeed.y = std::max(-m_ShipMaxSpeed, std::min(m_ShipMaxSpeed, m_ShipSpeed.y));

		// Update Position
		m_ShipPos += m_ShipSpeed * deltaTime;
		LoopPosition(m_ShipPos);

		// Update Firing
		// TODO: Current Implementation has a limit of one fire per simulation tick (Fine I guess)
		if (m_FireRate > 0.0f && m_TimeSinceLastFire > (1.0f / m_FireRate))
		{
			if (keyState.Pressed(KeyState::Keys::Space))
			{
				m_BulletsPositions[m_NextBulletIndex] = m_ShipPos;
				m_BulletsSpeed[m_NextBulletIndex] = direction * m_BulletSpeed;
				m_BulletsLife[m_NextBulletIndex] = m_BulletLifeSpan;

				if (++m_NextBulletIndex >= m_BulletsCount)
					m_NextBulletIndex = 0;
				m_TimeSinceLastFire = 0.0f;
			}
		}
		else
			m_TimeSinceLastFire += deltaTime;
	}

	void Game::LoopPosition(Vec2& position) const
	{
		// TODO: Optimize this
		if (position.x > m_ViewportSize.x) position.x = -m_ViewportSize.x;
		if (position.x < -m_ViewportSize.x) position.x = m_ViewportSize.x;
		if (position.y > m_ViewportSize.y) position.y = -m_ViewportSize.y;
		if (position.y < -m_ViewportSize.y) position.y = m_ViewportSize.y;
	}
}

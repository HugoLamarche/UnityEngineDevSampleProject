#include "AsteroidsAPI.h"

#include <AsteroidsGame.h>

#include <string>

namespace AsteroidsCPP
{
	ASTEROID_EXPORT void* AllocateGamePtr(float shipControlSpeed,
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
										  Vec2 viewportSize)
	{
		return new AsteroidsCPP::Game(shipControlSpeed,
									  shipControlRotationSpeed,
									  shipMaxSpeed,
									  shipSqrRadius,
									  asteroidTemplatesCount,
									  maxAsteroidsCount,
									  minAsteroidsSpeed,
									  maxAsteroidsSpeed,
									  fireRate,
									  bulletLifeSpan,
									  bulletSpeed,
									  bulletSqrRadius,
									  viewportSize);
	}

	ASTEROID_EXPORT void DetroyGamePtr(void* gamePtr)
	{
		if (gamePtr)
			delete gamePtr;
	}

	ASTEROID_EXPORT void SetAsteroidTemplateSqrRadius(void* gamePtr, std::uint32_t level, float sqrRadius)
	{
		static_cast<AsteroidsCPP::Game*>(gamePtr)->SetAsteroidTemplateSqrRadius(level, sqrRadius);
	}

	ASTEROID_EXPORT std::uint32_t GetAsteroidsCount(void* gamePtr)
	{
		return static_cast<AsteroidsCPP::Game*>(gamePtr)->GetAsteroidsCount();
	}

	ASTEROID_EXPORT std::uint32_t GetBulletsCount(void* gamePtr)
	{
		return static_cast<AsteroidsCPP::Game*>(gamePtr)->GetBulletsCount();
	}

	ASTEROID_EXPORT void Update(void* gamePtr, AsteroidsCPP::Game::KeyState keyState, float deltaTime)
	{
		static_cast<AsteroidsCPP::Game*>(gamePtr)->Update(keyState, deltaTime);
	}

	ASTEROID_EXPORT void GetAsteroidsPositions(void* gamePtr, Vec2** positions)
	{
		*positions = static_cast<AsteroidsCPP::Game*>(gamePtr)->GetAsteroidsPositions();
	}

	ASTEROID_EXPORT void GetBulletsPositions(void* gamePtr, Vec2** positions)
	{
		*positions = static_cast<AsteroidsCPP::Game*>(gamePtr)->GetBulletsPositions();
	}

	ASTEROID_EXPORT AsteroidsCPP::Vec3 GetShipPosRot(void* gamePtr)
	{
		return static_cast<AsteroidsCPP::Game*>(gamePtr)->GetShipPosRot();
	}
}

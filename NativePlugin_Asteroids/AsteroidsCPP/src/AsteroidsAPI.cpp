#include "AsteroidsAPI.h"

#include <AsteroidsGame.h>

#include <string>

namespace AsteroidsCPP
{
	ASTEROID_EXPORT void* AllocateGamePtr(float shipControlSpeed,
										  float shipControlRotationSpeed,
										  float shipMaxSpeed,
										  std::uint32_t asteroidTemplatesCount,
										  std::uint32_t maxAsteroidsCount,
										  float minAsteroidsSpeed,
										  float maxAsteroidsSpeed,
										  Vec2 viewportSize)
	{
		return new AsteroidsCPP::Game(shipControlSpeed, shipControlRotationSpeed, shipMaxSpeed, asteroidTemplatesCount, maxAsteroidsCount, minAsteroidsSpeed, maxAsteroidsSpeed, viewportSize);
	}

	ASTEROID_EXPORT void DetroyGamePtr(void* gamePtr)
	{
		if (gamePtr)
			delete gamePtr;
	}

	ASTEROID_EXPORT std::uint32_t GetAsteroidsCount(void* gamePtr)
	{
		return static_cast<AsteroidsCPP::Game*>(gamePtr)->GetAsteroidsCount();
	}

	ASTEROID_EXPORT void Update(void* gamePtr, AsteroidsCPP::Game::KeyState keyState, float deltaTime)
	{
		static_cast<AsteroidsCPP::Game*>(gamePtr)->Update(keyState, deltaTime);
	}

	ASTEROID_EXPORT void GetAsteroidsPositions(void* gamePtr, Vec2** positions)
	{
		*positions = static_cast<AsteroidsCPP::Game*>(gamePtr)->GetAsteroidsPositions();
	}

	ASTEROID_EXPORT AsteroidsCPP::Vec3 GetShipPosRot(void* gamePtr)
	{
		return static_cast<AsteroidsCPP::Game*>(gamePtr)->GetShipPosRot();
	}
}

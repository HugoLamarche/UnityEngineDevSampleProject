#pragma once

#ifdef _WIN32
#ifdef NOEXPORT
	#define ASTEROID_EXPORT 
#else
	#define ASTEROID_EXPORT __declspec(dllexport)
#endif
#else
    #define ASTEROID_EXPORT
#endif

#include <cstdint>
#include <AsteroidsGame.h>

extern "C"
{
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
											  Vec2 viewportSize);

		ASTEROID_EXPORT void DetroyGamePtr(void* gamePtr);

		ASTEROID_EXPORT void SetAsteroidTemplateSqrRadius(void* gamePtr, std::uint32_t level, float sqrRadius);

		ASTEROID_EXPORT std::uint32_t GetAsteroidsCount(void* gamePtr);

		ASTEROID_EXPORT std::uint32_t GetBulletsCount(void* gamePtr);

		ASTEROID_EXPORT void Update(void* gamePtr, AsteroidsCPP::Game::KeyState keyState, float deltaTime);

		ASTEROID_EXPORT void GetAsteroidsPositions(void* gamePtr, Vec2** positions);

		ASTEROID_EXPORT void GetBulletsPositions(void* gamePtr, Vec2** positions);

		ASTEROID_EXPORT AsteroidsCPP::Vec3 GetShipPosRot(void* gamePtr);
	}
}

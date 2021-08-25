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
		//Sample Functions
		ASTEROID_EXPORT const char* HelloWorld();

		ASTEROID_EXPORT std::uint32_t AddNumbers(std::uint32_t a, std::uint32_t b);

		ASTEROID_EXPORT void* AllocateGamePtr(float shipControlSpeed, float shipControlRotationSpeed, std::uint32_t asteroidTemplatesCount, std::uint32_t maxAsteroidsCount, Vec2 viewportSize)
		{
			return new AsteroidsCPP::Game(shipControlSpeed, shipControlRotationSpeed, asteroidTemplatesCount, maxAsteroidsCount, viewportSize);
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

		// TODO: Implement Asteroid API here
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
}

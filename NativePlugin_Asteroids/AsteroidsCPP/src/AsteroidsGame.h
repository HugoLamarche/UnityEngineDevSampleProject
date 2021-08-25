#pragma once

#include <cstdint>

namespace AsteroidsCPP
{
	struct Vec2
	{
		float x, y;

		Vec2 operator*(float a)
		{
			Vec2 result = *this;
			result.x *= a;
			result.y *= a;
			return result;
		}

		void operator+=(Vec2 a)
		{
			x += a.x;
			y += a.y;
		}
	};

	class Game
	{
	public:

		struct KeyState
		{
			enum class Keys : std::uint32_t
			{
				Left = 1 << 0,
				Right = 1 << 1,
				Up = 1 << 2,
				Down = 1 << 3,
				Space = 1 << 4,
			};

			std::uint32_t State;

			inline bool Pressed(Keys key) const
			{
				return (State & (std::uint32_t)key) != 0;
			}
		};

		Game(std::uint32_t asteroidTemplatesCount, std::uint32_t maxAsteroidsCount, const Vec2& viewportSize);
		~Game();

		void Update(KeyState state, float deltaTime);

		Vec2* GetAsteroidsPositions() const { return m_AsteroidsPositions; }
		std::uint32_t GetAsteroidsCount() const { return m_AsteroidsCount; }
		const Vec2 GetShipPos() const { return m_ShipPos; }

	private:
		uint32_t m_AsteroidTemplatesCount;
		uint32_t m_MaxAsteroidsCount;
		Vec2 m_ViewportSize;

		uint32_t m_AsteroidsCount;

		Vec2* m_AsteroidsPositions;
		Vec2* m_AsteroidsSpeeds;
		Vec2 m_ShipPos;
	};
}
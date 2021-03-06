#pragma once

#include <cstdint>

namespace AsteroidsCPP
{
	struct Vec2
	{
		float x, y;

		Vec2(float x = 0.0f, float y = 0.0f)
			: x(x)
			, y(y)
		{
		}

		Vec2 operator*(float a) const
		{
			Vec2 result = *this;
			result.x *= a;
			result.y *= a;
			return result;
		}

		void operator*=(const Vec2& a)
		{
			x *= a.x;
			y *= a.y;
		}

		void operator+=(const Vec2& a)
		{
			x += a.x;
			y += a.y;
		}
	};

	struct Vec3
	{
		float x, y, z;
		
		Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
			: x(x)
			, y(y)
			, z(z)
		{
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

		Game(float shipControlSpeed,
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
             const Vec2& viewportSize);
		~Game();

        void SetAsteroidTemplateSqrRadius(std::uint32_t level, float sqrRadius);

		void Update(KeyState state, float deltaTime);

		Vec2* GetAsteroidsPositions() const { return m_AsteroidsPositions; }
		std::uint32_t GetAsteroidsCount() const { return m_AsteroidsCount; }
		const Vec3 GetShipPosRot() const { return Vec3(m_ShipPos.x, m_ShipPos.y, m_ShipRot); }

		Vec2* GetBulletsPositions() const { return m_BulletsPositions; }
		std::uint32_t GetBulletsCount() const { return m_BulletsCount; }

	private:
        std::uint32_t GetLevelFromIndex(std::uint32_t index) const;

		void UpdateAsteroids(float deltaTime);
		void UpdateBullets(float deltaTime);
		void ApplyShipControl(KeyState state, float deltaTime);
		void LoopPosition(Vec2& position) const;
		void BreakAsteroid(const Vec2 position, uint32_t level);

		float m_ShipControlSpeed;
		float m_ShipControlRotationSpeed;
		float m_ShipMaxSpeed;
        float m_ShipSqrRadius;

        std::uint32_t m_BulletsCount;
        float m_FireRate;
        float m_BulletLifeSpan;
        float m_BulletSpeed;
        float m_BulletSqrRadius;

		uint32_t m_AsteroidTemplatesCount;
        float* m_AsteroidsTemplateSqrRadius;

		uint32_t m_MaxAsteroidsCount;
        float m_MinAsteroidsSpeed;
        float m_MaxAsteroidsSpeed;
		Vec2 m_ViewportSize;

		uint32_t m_AsteroidsCount;
        uint32_t* m_LevelsBoundaries;

		Vec2* m_AsteroidsPositions;
		Vec2* m_AsteroidsSpeeds;

		float m_TimeSinceLastFire;
		uint32_t m_NextBulletIndex;
		Vec2* m_BulletsPositions;
		Vec2* m_BulletsSpeed;
		float* m_BulletsLife;

		Vec2 m_ShipPos;
		float m_ShipRot;
		Vec2 m_ShipSpeed;
        bool m_ShipDestroyed;
	};
}
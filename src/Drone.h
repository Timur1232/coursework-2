#pragma once

#include <SFML/Graphics.hpp>

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "Beacon.h"

namespace CW {

	class DroneDebugInterface;

	class Drone
		: public IUpdate,
		  public IDrawable
	{
	public:
		Drone(sf::Vector2f position, sf::Angle directionAngle = sf::Angle::Zero);

		static void staticInit();

		static void debugInterface();
		void infoInterface(size_t index, bool* open) const;

		void update(sf::Time deltaTime) override;
		void draw(sf::RenderWindow& render) const override;

		void reactToBeacons(const std::vector<Beacon*>& beacons);

	private:
		friend DroneDebugInterface;

		inline void turn(sf::Time deltaTime);
		inline void wander(sf::Time deltaTime);

		// Debug
		inline void setMeshPos(sf::Vector2f position) const;

	private:
		static constexpr sf::Vector2f ONE_LENGTH_VEC = { 1.0f, 0.0f };

		sf::Vector2f m_Position;
		sf::Angle m_DirectionAngle = sf::Angle::Zero;
		sf::Angle m_AttractionAngle;

		TargetType m_TargetType = TargetType::Recource;

		static float s_Speed;
		static sf::Angle s_TurningSpeed;
		static float s_FOV;
		static sf::Vector2f s_ViewDistanse;

		static float s_BeaconCooldownSec;
		static float s_WanderCooldownSec;

		static sf::Angle s_RandomWanderAngle;
		static sf::Angle s_WanderAngleThreshold;
		static sf::Angle s_MaxTurningDelta;

		float m_BeaconTimerSec = s_BeaconCooldownSec;
		float m_WanderTimer = 0.0f;

		// Debug
		static sf::CircleShape s_Mesh;
		static sf::CircleShape s_MeshViewDistance;
		static sf::CircleShape s_DirectionVisual;
		static sf::CircleShape s_AttractionAngleVisual;

		static bool s_DrawViewDistance;
		static bool s_DrawDirection;
	};


	/*class DroneDebugInterface
	{
	public:
		DroneDebugInterface() = default;

		void updateInterface() const;
	};*/

} // CW
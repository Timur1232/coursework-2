#pragma once

#include <SFML/Graphics.hpp>

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "Beacon.h"

namespace CW {

	class Drone
		: public CW_E::IUpdate,
		  public CW_E::IDrawable
	{
	public:
		Drone(sf::Vector2f position, sf::Angle directionAngle = sf::Angle::Zero);

		void update(sf::Time deltaTime) override;
		void draw(sf::RenderWindow& render) const override;

		void reactToBeacons(const std::vector<Beacon>& beacons);
		BeaconType oppositeBeaconType() const;

	private:
		void turn(sf::Time deltaTime);
		void wander(sf::Time deltaTime);

		// Debug
		void setMeshPos(sf::Vector2f position);

	private:
		static constexpr sf::Vector2f ONE_LENGTH_VEC = { 1.0f, 0.0f };

		sf::Vector2f m_Position;
		sf::Angle m_DirectionAngle = sf::Angle::Zero;
		sf::Angle m_AtractedAngle;

		BeaconType m_TargetBeaconType = BeaconType::Recource;

		static float s_Speed;
		static float s_TurningSpeed;
		static float s_FOV;
		static sf::Vector2f s_ViewDistanse;
		static float s_BeaconCooldownSec;
		static float s_WanderCooldownSec;
		static sf::Angle s_WanderAngle;

		float m_BeaconTimerSec = s_BeaconCooldownSec;
		float m_WanderTimer = 0.0f;

		// TODO: Система ресурсов
		sf::CircleShape m_Mesh{ 50.0f };
		sf::CircleShape m_MeshFOV;
		sf::CircleShape m_DirectionVisual{5.f};
	};

} // CW
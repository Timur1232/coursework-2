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
		Drone(sf::Vector2f position, sf::Vector2f direction);

		void update(sf::Time deltaTime) override;
		void draw(sf::RenderWindow& render) const override;

		void reactToBeacons(const std::vector<Beacon>& beacons);

	private:
		BeaconType oppositeBeaconType() const;

		// Debug
		void setMeshPos(sf::Vector2f position);

	private:
		sf::Vector2f m_Position;
		sf::Vector2f m_Direction;

		BeaconType m_TargetBeaconType = BeaconType::Recource;

		static float s_Speed;
		static float s_FOV;
		static sf::Vector2f s_ViewDistanse;
		static float s_BeaconCooldownSec;
		float m_BeaconTimerSec = s_BeaconCooldownSec;

		// TODO: Система ресурсов
		sf::CircleShape m_Mesh{ 50.0f };
		sf::CircleShape m_MeshFOV;
		sf::CircleShape m_DirectionVisual{5.f};
	};

} // CW
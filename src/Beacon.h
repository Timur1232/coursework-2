#pragma once

#include <SFML/System/Vector2.hpp>

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "engine/Events.h"

#include "BeaconType.h"

namespace CW {

	class Beacon
		: public CW_E::IUpdate,
		  public CW_E::IDrawable
	{
	public:
		Beacon(sf::Vector2f position, BeaconType type);

		void update(sf::Time deltaTime) override;
		void draw(sf::RenderWindow& render) const override;

		bool isAlive() const;

		void revive(sf::Vector2f newPosition, BeaconType newType);

		sf::Vector2f getPos() const;
		BeaconType getType() const;

		void setBeaconColor(BeaconType type);

	private:
		BeaconType m_Type;
		bool m_Alive = true;

		sf::Vector2f m_Position;
		float m_Charge = 1.0f;
		static float s_ChargeThreshold;
		static float s_DischargeRate;

		// TODO: сделать систему управления ресурсами
		sf::CircleShape m_Mesh{10.0f};
		sf::Color m_Color;
	};

} // CW
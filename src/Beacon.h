#pragma once

#include <SFML/System/Vector2.hpp>

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"

#include "TargetType.h"

namespace CW {

	class Beacon
		: public IUpdate,
		  public IDrawable
	{
	public:
		Beacon(sf::Vector2f position, TargetType type);

		void update(sf::Time deltaTime) override;
		void draw(sf::RenderWindow& render) const override;

		bool isAlive() const;

		void revive(sf::Vector2f newPosition, TargetType newType);

		sf::Vector2f getPos() const;
		TargetType getType() const;

	private:
		sf::Color beaconColor() const;

	private:
		TargetType m_Type;
		bool m_Alive = true;

		sf::Vector2f m_Position;
		float m_Charge = 1.0f;
		static float s_ChargeThreshold;
		static float s_DischargeRate;

		static sf::CircleShape s_Mesh;
	};

} // CW
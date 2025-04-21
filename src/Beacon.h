#pragma once

#include "pch.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"

#include "TargetType.h"

namespace CW {

	class Beacon
		: public IUpdate,
		  public IDrawable
	{
	public:
		Beacon() = default;
		Beacon(sf::Vector2f position, TargetType type);

		static void StaticInit();

		static void DebugInterface();
		void InfoInterface(size_t index, bool* open) const;

		void Update(sf::Time deltaTime) override;
		void Draw(sf::RenderWindow& render) const override;

		bool IsAlive() const;

		void Revive(sf::Vector2f newPosition, TargetType newType);

		sf::Vector2f GetPos() const;
		TargetType GetType() const;

	private:
		sf::Color beaconColor() const;

	private:
		TargetType m_Type = TargetType::None;
		bool m_Alive = true;

		sf::Vector2f m_Position;
		float m_Charge = 1.0f;
		static float s_ChargeThreshold;
		static float s_DischargeRate;

		static sf::CircleShape s_Mesh;
	};

} // CW
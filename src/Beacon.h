#pragma once

#include "pch.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "engine/Object.h"

#include "TargetType.h"
#include "BitDirection.h"

namespace CW {

	class Beacon
		: public Object,
		  public IUpdate,
		  public IDrawable
	{
	public:
		Beacon() = default;
		Beacon(sf::Vector2f position, TargetType type, uint8_t bitDirection);

		static void StaticInit();

		static void DebugInterface();
		void InfoInterface(size_t index, bool* open) const;

		void Update(sf::Time deltaTime) override;
		void Draw(sf::RenderWindow& render) const override;

		[[nodiscard]] inline bool IsAlive() const { return m_Alive; }

		void Revive(sf::Vector2f newPosition, TargetType newType, uint8_t bitDirection);

		[[nodiscard]] inline TargetType GetType() const { return m_Type; }
		[[nodiscard]] inline uint8_t GetBitDirection() const { return m_BitDirection; }
		[[nodiscard]] sf::Angle GetDirectionAngle() const;

	private:
		[[nodiscard]] sf::Color beaconColor() const;

	private:
		TargetType m_Type = TargetType::None;
		uint8_t m_BitDirection = DirectionBit::None;
		float m_Charge = 1.0f;
		bool m_Alive = true;

		static float s_ChargeThreshold;
		static float s_DischargeRate;

		static sf::CircleShape s_Mesh;
	};

} // CW
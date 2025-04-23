#pragma once

#include "pch.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "engine/Object.h"

#include "TargetType.h"

namespace CW {

	class Beacon
		: public Object,
		  public IUpdate,
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

		[[nodiscard]] inline bool IsAlive() const { return m_Alive; }

		void Revive(sf::Vector2f newPosition, TargetType newType);

		[[nodiscard]] inline TargetType GetType() const { return m_Type; }

	private:
		[[nodiscard]] sf::Color beaconColor() const;

	private:
		TargetType m_Type = TargetType::None;
		float m_Charge = 1.0f;
		bool m_Alive = true;

		static float s_ChargeThreshold;
		static float s_DischargeRate;

		static sf::CircleShape s_Mesh;
	};

} // CW
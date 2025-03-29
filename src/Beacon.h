#pragma once

#include <SFML/System/Vector2.hpp>

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "engine/Events.h"

namespace CW {

	enum class BeconType
	{
		None = 0,
		Navigation,
		Recource
	};

	class Beacon
		: public CW_E::IUpdate,
		  public CW_E::IDrawable
	{
	public:
		Beacon(size_t index, sf::Vector2f position,
			CW_E::UpdateHandlerWrapper u_h, CW_E::EventHandlerWrapper e_h);
		~Beacon();

		void update(sf::Time deltaTime) override;
		void draw(CW_E::RenderWrapper render) const override;

		bool isAlive() const;

	private:
		size_t m_Index;
		size_t m_UIndex;
		CW_E::UpdateHandlerWrapper m_UpdateHandler;
		CW_E::EventHandlerWrapper m_EventHandler;

		bool m_Alive = true;

		sf::Vector2f m_Position;
		float m_Charge = 1.0f;
		float m_ChargeThreshold = 0.1f;
		float m_DischargeRate = 0.1f;

		// TODO: сделать систему управления ресурсами
		sf::CircleShape m_Mesh{10.0f};
		sf::Color m_Color{255, 255, 255, 255};
	};

} // CW
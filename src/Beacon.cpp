#include "Beacon.h"

#include "utils/utils.h"
#include <imgui.h>

namespace CW {

	Beacon::Beacon(size_t index, sf::Vector2f position,
		CW_E::UpdateHandlerWrapper u_h, CW_E::EventHandlerWrapper e_h)
		: m_Index(index), m_UpdateHandler(u_h), m_EventHandler(e_h), m_Position(position)
	{
		m_UIndex = u_h.subscribe(this);
		m_Mesh.setPosition(m_Position);
	}

	Beacon::~Beacon()
	{
		m_UpdateHandler.unsubscribe(m_UIndex);
	}

	void Beacon::update(sf::Time deltaTime)
	{
		if (!isAlive())
			return;

		if (m_Charge <= m_ChargeThreshold)
		{
			m_Charge = 0.0f;
			m_Color.a = 0;
			m_EventHandler.addEvent<BeaconDischarge>(BeaconDischarge{m_Index});
			m_Alive = false;
		}
		else
		{
			m_Charge = CW_E::lerp(0.0f, 1.0f, m_Charge - m_DischargeRate * deltaTime.asSeconds());
			m_Color.a = m_Charge * 255;
			m_Mesh.setFillColor(m_Color);
		}

		ImGui::Begin("Beacons");
		ImGui::Text("beacon position: (%.2f, %.2f)", m_Position.x, m_Position.y);
		ImGui::Text("beacon charge: %.2f", m_Charge);
		ImGui::End();
	}

	void Beacon::draw(CW_E::RenderWrapper render) const
	{
		if (isAlive())
			render.draw(m_Mesh);
	}

	bool Beacon::isAlive() const
	{
		return m_Alive;
	}

} // CW
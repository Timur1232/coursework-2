#include "Beacon.h"

#include "utils/utils.h"
#include <imgui.h>

namespace CW {

	float Beacon::s_ChargeThreshold = 0.05f;
	float Beacon::s_DischargeRate = 0.02f;

	Beacon::Beacon(sf::Vector2f position, BeaconType type)
		: m_Position(position), m_Type(type)
	{
		m_Mesh.setOrigin({ m_Mesh.getRadius(), m_Mesh.getRadius() });
		m_Mesh.setPosition(m_Position);

		setBeaconColor(m_Type);
	}

	void Beacon::update(sf::Time deltaTime)
	{
		if (!isAlive())
			return;

		if (m_Charge <= s_ChargeThreshold)
		{
			m_Charge = 0.0f;
			m_Color.a = 0;
			//CW_E::EventHandler::get().addEvent<BeaconDischarge>(BeaconDischarge{m_Index});
			m_Alive = false;
		}
		else
		{
			m_Charge = CW_E::lerp(0.0f, 1.0f, m_Charge - s_DischargeRate * deltaTime.asSeconds());
			m_Color.a = m_Charge * 255;
			m_Mesh.setFillColor(m_Color);
		}

		ImGui::Begin("Beacons");
		ImGui::SliderFloat("charge threshold", &s_ChargeThreshold, 0.0f, 1.0f);
		ImGui::SliderFloat("discharge rate", &s_DischargeRate, 0.0f, 1.0f);
		ImGui::Text("beacon position: (%.2f, %.2f)", m_Position.x, m_Position.y);
		ImGui::Text("beacon charge: %.2f", m_Charge);
		ImGui::End();
	}

	void Beacon::draw(sf::RenderWindow& render) const
	{
		if (isAlive())
			render.draw(m_Mesh);
	}

	bool Beacon::isAlive() const
	{
		return m_Alive;
	}

	void Beacon::revive(sf::Vector2f newPosition, BeaconType newType)
	{
		m_Charge = 1.0f;
		m_Position = newPosition;
		m_Type = newType;
		setBeaconColor(m_Type);
		m_Mesh.setPosition(m_Position);
		m_Alive = true;
	}

	sf::Vector2f Beacon::getPos() const
	{
		return m_Position;
	}

	BeaconType Beacon::getType() const
	{
		return m_Type;
	}

	void Beacon::setBeaconColor(BeaconType type)
	{
		switch (m_Type)
		{
		case BeaconType::Navigation:	m_Color = sf::Color::White; break;
		case BeaconType::Recource:		m_Color = sf::Color::Green; break;
		default:						m_Color = sf::Color::Red; break;
		}
	}

} // CW
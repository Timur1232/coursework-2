#include "Beacon.h"

#include "utils/utils.h"
#include <imgui.h>

#include "debug_utils/Profiler.h"

namespace CW {

	sf::CircleShape Beacon::s_Mesh;

	float Beacon::s_ChargeThreshold = 0.05f;
	float Beacon::s_DischargeRate = 0.02f;

	Beacon::Beacon(sf::Vector2f position, TargetType type)
		: m_Position(position), m_Type(type)
	{
		s_Mesh.setPosition(m_Position);
	}

	void Beacon::staticInit()
	{
		s_Mesh.setRadius(10.0f);
		s_Mesh.setOrigin({ s_Mesh.getRadius(), s_Mesh.getRadius() });
		s_Mesh.setPointCount(4);
	}

	void Beacon::debugInterface()
	{
		ImGui::SliderFloat("charge threshold", &s_ChargeThreshold, 0.0f, 1.0f);
		ImGui::SliderFloat("discharge rate", &s_DischargeRate, 0.0f, 1.0f);
	}

	void Beacon::infoInterface(size_t index, bool* open) const
	{
		ImGui::Begin("Beacons", open);
		ImGui::Separator();
		ImGui::Text("index: %d", index);
		ImGui::Text("alive: %d", m_Alive);
		ImGui::Text("beacon position: (%.2f, %.2f)", m_Position.x, m_Position.y);
		ImGui::Text("beacon charge: %.2f", m_Charge);
		ImGui::End();
	}

	void Beacon::update(sf::Time deltaTime)
	{
		if (!isAlive())
			return;

		if (m_Charge <= s_ChargeThreshold)
		{
			m_Charge = 0.0f;
			m_Alive = false;
		}
		else
		{
			m_Charge -= s_DischargeRate * deltaTime.asSeconds();
		}
	}

	void Beacon::draw(sf::RenderWindow& render) const
	{
		if (isAlive())
		{
			s_Mesh.setPosition(m_Position);
			s_Mesh.setFillColor(beaconColor());
			render.draw(s_Mesh);
		}
	}

	bool Beacon::isAlive() const
	{
		return m_Alive;
	}

	void Beacon::revive(sf::Vector2f newPosition, TargetType newType)
	{
		m_Charge = 1.0f;
		m_Position = newPosition;
		m_Type = newType;
		m_Alive = true;
	}

	sf::Vector2f Beacon::getPos() const
	{
		return m_Position;
	}

	TargetType Beacon::getType() const
	{
		return m_Type;
	}

	sf::Color Beacon::beaconColor() const
	{
		sf::Color color;
		switch (m_Type)
		{
		case TargetType::Navigation:	color = sf::Color::White; break;
		case TargetType::Recource:		color = sf::Color::Green; break;
		default:						color = sf::Color::Red; break;
		}

		if (m_Alive)
		{
			color.a = (uint8_t)(m_Charge * 255);
		}
		else
		{
			color.a = 0;
		}

		return color;
	}

} // CW
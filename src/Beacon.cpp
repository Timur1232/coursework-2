#include "pch.h"
#include "Beacon.h"

#include "utils/utils.h"
#include "debug_utils/Profiler.h"
#include "BitDirection.h"

namespace CW {

	sf::CircleShape Beacon::s_Mesh;

	float Beacon::s_ChargeThreshold = 0.05f;
	float Beacon::s_DischargeRate = 0.2f;

	Beacon::Beacon(sf::Vector2f position, TargetType type, uint8_t bitDirection)
		: Object(position), m_Type(type), m_BitDirection(bitDirection)
	{
		s_Mesh.setPosition(m_Position);
	}

	void Beacon::StaticInit()
	{
		s_Mesh.setRadius(10.0f);
		s_Mesh.setOrigin({ s_Mesh.getRadius(), s_Mesh.getRadius() });
		s_Mesh.setPointCount(4);
	}

	void Beacon::DebugInterface()
	{
		ImGui::SliderFloat("charge threshold", &s_ChargeThreshold, 0.0f, 1.0f);
		ImGui::SliderFloat("discharge rate", &s_DischargeRate, 0.1f, 100.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
	}

	void Beacon::InfoInterface(size_t index, bool* open) const
	{
		ImGui::Begin("Beacons", open);
		ImGui::Separator();
		ImGui::Text("index: %d", index);
		ImGui::Text("alive: %d", m_Alive);
		ImGui::Text("beacon position: (%.2f, %.2f)", m_Position.x, m_Position.y);
		ImGui::Text("beacon charge: %.3f", m_Charge);
		ImGui::End();
	}

	void Beacon::Update(sf::Time deltaTime)
	{
		if (!IsAlive())
			return;

		if (m_Charge <= s_ChargeThreshold)
		{
			m_Charge = 0.0f;
			m_Alive = false;
		}
		else
		{
			m_Charge -= s_DischargeRate / 100.0f * deltaTime.asSeconds();
		}
	}

	void Beacon::Draw(sf::RenderWindow& render) const
	{
		if (IsAlive())
		{
			s_Mesh.setPosition(m_Position);
			s_Mesh.setFillColor(beaconColor());
			render.draw(s_Mesh);
		}
	}

	void Beacon::Revive(sf::Vector2f newPosition, TargetType newType, uint8_t bitDirection)
	{
		m_Charge = 1.0f;
		m_Position = newPosition;
		m_Type = newType;
		m_Alive = true;
		m_BitDirection = bitDirection;
	}

	sf::Angle Beacon::GetDirectionAngle() const
	{
		if (m_BitDirection == None)
		{
			return sf::Angle::Zero;
		}
		return DIRECTION_ANGLE_TABLE.at(m_BitDirection);
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
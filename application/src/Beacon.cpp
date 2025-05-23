#include "pch.h"
#include "Beacon.h"

#include "utils/utils.h"
#include "debug_utils/Profiler.h"
#include "BitDirection.h"
#include "engine/Renderer.h"

#include "SimState.h"

namespace CW {

	Beacon::Beacon(sf::Vector2f position, TargetType type, uint8_t bitDirection, float charge)
		: Object(position), m_Type(type), m_BitDirection(bitDirection), m_Charge(charge)
	{
	}

	void Beacon::InfoInterface(size_t index) const
	{
		ImGui::Separator();
		ImGui::Text("index: %d", index);
		ImGui::Text("alive: %d", m_Alive);
		ImGui::Text("beacon position: (%.2f, %.2f)", m_Position.x, m_Position.y);
		ImGui::Text("beacon charge: %.3f", m_Charge);
		ImGui::Text("beacon direction: %d", m_BitDirection);
	}

	void Beacon::Update(float deltaTime, const BeaconSettings& bs)
	{
		if (!IsAlive())
			return;

		if (m_Charge <= bs.ChargeThreshold)
		{
			m_Charge = 0.0f;
			m_Alive = false;
		}
		else
		{
			m_Charge -= bs.DischargeRate / 100.0f * deltaTime;
		}
	}

	void Beacon::Revive(sf::Vector2f newPosition, TargetType newType, uint8_t bitDirection, float charge)
	{
		m_Charge = 1.0f;
		m_Position = newPosition;
		m_Type = newType;
		m_Alive = true;
		m_Charge = charge;
		m_BitDirection = bitDirection;
	}

	sf::Angle Beacon::GetDirectionAngle() const
	{
		if (m_BitDirection == DirectionBit::None)
		{
			return sf::Angle::Zero;
		}
		return DIRECTION_ANGLE_TABLE.at(m_BitDirection);
	}

	sf::Color Beacon::BeaconColor() const
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


	BeaconManager::BeaconManager()
	{
		m_Beacons.reserve(1024 * 1024);
	}

	BeaconManager::BeaconManager(const BeaconSettings& settings)
		: BeaconManager()
	{
		m_BeaconSettings = settings;
	}

	void BeaconManager::SetState(FullSimulationState& state)
	{
		Clear();
		m_BeaconSettings = state.Settings.Beacons;
		for (const auto& beacon : state.Beacons)
		{
			CreateBeacon(beacon.GetPos(), beacon.GetType(), beacon.GetBitDirection(), beacon.GetCharge());
		}
	}

	void BeaconManager::CollectState(SimulationState& state) const
	{
		for (const auto& beacon : m_Beacons)
		{
			state.BeaconsPositions.push_back(beacon->GetPos());
			state.BeaconsTypes.push_back(beacon->GetType());
			state.BeaconsCharges.push_back(beacon->GetCharge());
		}
	}

	void BeaconManager::CollectState(FullSimulationState& state) const
	{
		for (const auto& beacon : m_Beacons)
			if (beacon->IsAlive())
				state.Beacons.push_back(beacon.Object);
	}

	void BeaconManager::SetSettings(const BeaconSettings& settings)
	{
		m_BeaconSettings = settings;
	}

	void BeaconManager::DebugInterface()
	{
		ImGui::SliderFloat("charge threshold", &m_BeaconSettings.ChargeThreshold, 0.0f, 1.0f);
		ImGui::SliderFloat("discharge rate", &m_BeaconSettings.DischargeRate, 0.1f, 100.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
	}

	void BeaconManager::Update(float deltaTime)
	{
		for (size_t i = 0; i < m_Beacons.size() - m_DeadBeacons; ++i)
		{
			auto& beacon = m_Beacons[i];

			beacon->Update(deltaTime, m_BeaconSettings);

			if (!beacon->IsAlive())
			{
				m_Chunks.ForgetObject(beacon);
				++m_DeadBeacons;
				std::swap(m_Beacons[i], m_Beacons[m_Beacons.size() - m_DeadBeacons]);
			}
		}
	}

	void BeaconManager::DrawAllBeacons()
	{
		auto& circleBuilder = Renderer::Get().BeginCircleShape();
		circleBuilder.PointCount(4)
			.Radius(10.0f);
		for (auto& beacon : m_Beacons)
		{
			if (beacon->IsAlive())
			{
				circleBuilder.Position(beacon->GetPos())
					.Color(beacon->BeaconColor())
					.Draw();
			}
		}
		circleBuilder.SetDefault();
	}

	void BeaconManager::CreateBeacon(sf::Vector2f position, TargetType type, uint8_t bitDirection, float charge)
	{
		CW_PROFILE_FUNCTION();
		if (m_DeadBeacons)
		{
			IndexedBeacon& beacon = m_Beacons[m_Beacons.size() - m_DeadBeacons];
			beacon->Revive(position, type, bitDirection, charge);
			m_Chunks.AddObject(beacon);
			--m_DeadBeacons;
		}
		else
		{
			bool realloc = m_Beacons.capacity() - m_Beacons.size() == 0;
			if (realloc)
			{
				for (auto& beacon : m_Beacons)
				{
					m_Chunks.ForgetObject(beacon);
				}
			}
			m_Beacons.emplace_back(position, type, bitDirection, charge);
			if (realloc)
			{
				for (size_t i = 0; i < m_Beacons.size() - 1; ++i)
				{
					m_Chunks.AddObject(m_Beacons[i]);
				}
			}
			IndexedBeacon& newBeacon = m_Beacons.back();
			m_Chunks.AddObject(newBeacon);
		}
	}

	void BeaconManager::Clear()
	{
		m_Chunks.Clear();
		m_Beacons.clear();
		m_DeadBeacons = 0;
		m_Beacons.reserve(BEACONS_RESERVE);
	}

	void BeaconManager::InfoInterface(bool* open)
	{
		ImGui::Begin("Beacons", open);
		size_t index = 0;
		for (auto& beacon : m_Beacons)
		{
			beacon->InfoInterface(index);
			++index;
		}
		ImGui::End();
	}

} // CW
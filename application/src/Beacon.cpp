#include "pch.h"
#include "Beacon.h"

#include "utils/utils.h"
#include "debug_utils/Profiler.h"
#include "BitDirection.h"
#include "engine/Renderer.h"

#include "SimState.h"

namespace CW {

	Beacon::Beacon(sf::Vector2f position, TargetType type, sf::Angle angle, float charge)
		: Object(position), m_Type(type), m_Direction(angle), m_Charge(charge)
	{
	}

	void Beacon::WriteToFile(std::ofstream& file) const
	{
		file.write(reinterpret_cast<const char*>(&m_Position), sizeof(m_Position));
		int typeInt = static_cast<int>(m_Type);
		file.write(reinterpret_cast<const char*>(&typeInt), sizeof(typeInt));
		file.write(reinterpret_cast<const char*>(&m_Charge), sizeof(m_Charge));
		float directionRad = m_Direction.asRadians();
		file.write(reinterpret_cast<const char*>(&directionRad), sizeof(directionRad));
	}

	void Beacon::ReadFromFile(std::ifstream& file)
	{
		file.read(reinterpret_cast<char*>(&m_Position), sizeof(m_Position));
		int typeInt;
		file.read(reinterpret_cast<char*>(&typeInt), sizeof(typeInt));
		m_Type = static_cast<TargetType>(typeInt);
		file.read(reinterpret_cast<char*>(&m_Charge), sizeof(m_Charge));
		float directionRad;
		file.read(reinterpret_cast<char*>(&directionRad), sizeof(directionRad));
		m_Direction = sf::radians(directionRad);
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

	void CW::Beacon::Revive(sf::Vector2f newPosition, TargetType newType, float charge)
	{
		m_Charge = 1.0f;
		m_Position = newPosition;
		m_Type = newType;
		m_Alive = true;
		m_Charge = charge;
	}

	sf::Angle Beacon::GetDirectionAngle() const
	{
		return DIRECTION_ANGLE_TABLE.at(angle_to_bit_direction(m_Direction));
	}


	BeaconManager::BeaconManager(const BeaconSettings& settings, float chunckSize)
		: m_Chunks(chunckSize)
	{
		m_BeaconSettings = settings;
		m_Beacons.reserve(1024 * 1024);
	}

	void BeaconManager::SetState(FullSimulationState& state)
	{
		Clear();
		m_BeaconSettings = state.Settings.Beacons;
		for (const auto& beacon : state.Beacons)
		{
			CreateBeacon(beacon.GetPos(), beacon.GetType(), beacon.GetDirectionAngle(), beacon.GetCharge());
		}
	}

	void CW::BeaconManager::CollectState(SimulationState& state, bool debug) const
	{
		for (const auto& beacon : m_Beacons)
		{
			if (beacon->IsAlive())
			{
				state.BeaconsPositions.push_back(beacon->GetPos());
				state.BeaconsTypes.push_back(beacon->GetType());
				state.BeaconsCharges.push_back(beacon->GetCharge());
			}
		}
		if (debug)
		{
			for (auto& chunk : m_Chunks.GetAllChunks())
			{
				state.ChunksPositions.push_back((sf::Vector2f) chunk.GetKey() * m_Chunks.GetChunkSize());
			}
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

	void BeaconManager::Update(float deltaTime)
	{
		for (size_t i = 0; i < m_Beacons.size() - m_DeadBeacons; ++i)
		{
			auto& beacon = m_Beacons[i];

			beacon->Update(deltaTime, m_BeaconSettings);

			if (!beacon->IsAlive())
			{
				++m_DeadBeacons;
				auto& last = m_Beacons[m_Beacons.size() - m_DeadBeacons];
				m_Chunks.ForgetObject(last);
				m_Chunks.ForgetObject(beacon);
				std::swap(beacon, last);
				m_Chunks.AddObject(beacon);
				--i;
			}
		}
	}

	void BeaconManager::CreateBeacon(sf::Vector2f position, TargetType type, sf::Angle angle, float charge)
	{
		CW_PROFILE_FUNCTION();
		if (m_DeadBeacons)
		{
			IndexedBeacon& beacon = m_Beacons[m_Beacons.size() - m_DeadBeacons];
			beacon->Revive(position, type, charge);
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
				CW_INFO("Beacon buffer realloc");
			}
			m_Beacons.emplace_back(position, type, angle, charge);
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

	sf::Color beacon_color(TargetType type, float charge)
	{
		sf::Color color;
		switch (type)
		{
		case TargetType::Navigation:	color = sf::Color::White; break;
		case TargetType::Recource:		color = sf::Color::Green; break;
		default:						color = sf::Color::Red; break;
		}
		color.a = (uint8_t) (charge * 255);
		return color;
	}

} // CW
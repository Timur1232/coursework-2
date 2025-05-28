#pragma once

#include "pch.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "engine/Object.h"
#include "utils/utils.h"

#include "Chunks.h"
#include "TargetType.h"
#include "BitDirection.h"
#include "BeaconSettings.h"

namespace CW {

	[[nodiscard]] sf::Color beacon_color(TargetType type, float charge);

	class Beacon
		: public Object
	{
	public:
		Beacon() = default;
		Beacon(sf::Vector2f position, TargetType type, sf::Angle angle, float charge = 1.0f);

		void WriteToFile(std::ofstream& file) const;
		void ReadFromFile(std::ifstream& file);

		void Update(float deltaTime, const BeaconSettings& bs);

		[[nodiscard]] bool IsAlive() const { return m_Alive; }

		void Revive(sf::Vector2f newPosition, TargetType newType, float charge = 1.0f);

		float GetCharge() const { return m_Charge; }
		[[nodiscard]] TargetType GetType() const { return m_Type; }
		[[nodiscard]] sf::Angle GetDirectionAngle() const;
		[[nodiscard]] sf::Angle GetBitDirectionAngle() const;
		void SetDirection(sf::Angle angle) { m_Direction = angle; }

	private:
		TargetType m_Type = TargetType::None;
		float m_Charge = 1.0f;
		sf::Angle m_Direction;
		bool m_Alive = true;
	};


	struct SimulationState;
	struct FullSimulationState;

	class BeaconManager
		: public IUpdate
	{
	public:
		using IndexedBeacon = Indexed<Beacon, 2>;

	public:
		BeaconManager(const BeaconSettings& settings, float chunckSize);

		void SetState(FullSimulationState& state);

		const std::vector<IndexedBeacon>& GetIndexedBeacons() const { return m_Beacons; }

		void CollectState(SimulationState& state, bool debug) const;
		void CollectState(FullSimulationState& state) const;

		void SetSettings(const BeaconSettings& settings);

		void Update(float deltaTime) override;
		void CreateBeacon(sf::Vector2f position, TargetType type, sf::Angle angle, float charge = 1.0f);
		void Clear();

		[[nodiscard]] const ChunkHandler<Beacon>& GetChuncks() const { return m_Chunks; }
		[[nodiscard]] size_t Size() const { return m_Beacons.size(); }
		[[nodiscard]] size_t Capacity() const { return m_Beacons.capacity(); }

	private:
		std::vector<IndexedBeacon> m_Beacons;
		ChunkHandler<Beacon> m_Chunks;
		size_t m_DeadBeacons = 0;

		BeaconSettings m_BeaconSettings;

		static constexpr size_t BEACONS_RESERVE = 1024 * 1024;
	};

} // CW
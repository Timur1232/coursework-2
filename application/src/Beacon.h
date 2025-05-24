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
		Beacon(sf::Vector2f position, TargetType type, uint8_t bitDirection, float charge = 1.0f);

		void WriteToFile(std::ofstream& file) const;
		void ReadFromFile(std::ifstream& file);

		void InfoInterface(size_t index) const;

		void Update(float deltaTime, const BeaconSettings& bs);

		[[nodiscard]] bool IsAlive() const { return m_Alive; }

		void Revive(sf::Vector2f newPosition, TargetType newType, uint8_t bitDirection, float charge = 1.0f);

		float GetCharge() const { return m_Charge; }
		[[nodiscard]] TargetType GetType() const { return m_Type; }
		[[nodiscard]] uint8_t GetBitDirection() const { return m_BitDirection; }
		[[nodiscard]] sf::Angle GetDirectionAngle() const;

	private:
		TargetType m_Type = TargetType::None;
		float m_Charge = 1.0f;
		byte m_BitDirection = DirectionBit::None;
		bool m_Alive = true;
	};


	struct SimulationState;
	struct FullSimulationState;

	class BeaconManager
		: public IUpdate
	{
	public:
		using IndexedBeacon = Indexed<Beacon>;

	public:
		BeaconManager();
		BeaconManager(const BeaconSettings& settings);

		void SetState(FullSimulationState& state);

		const std::vector<IndexedBeacon>& GetIndexedBeacons() const { return m_Beacons; }

		void CollectState(SimulationState& state) const;
		void CollectState(FullSimulationState& state) const;

		void SetSettings(const BeaconSettings& settings);

		void Update(float deltaTime) override;
		void DrawAllBeacons();
		void CreateBeacon(sf::Vector2f position, TargetType type, uint8_t bitDirection, float charge = 1.0f);
		void Clear();

		[[nodiscard]] const ChunkHandler<Beacon>& GetChuncks() const { return m_Chunks; }
		[[nodiscard]] size_t Size() const { return m_Beacons.size(); }
		[[nodiscard]] size_t Capacity() const { return m_Beacons.capacity(); }
		
		void InfoInterface(bool* open);
		void DebugInterface();

	private:
		std::vector<IndexedBeacon> m_Beacons;
		ChunkHandler<Beacon> m_Chunks{ 500.0f };
		size_t m_DeadBeacons = 0;

		BeaconSettings m_BeaconSettings;

		static constexpr size_t BEACONS_RESERVE = 1024 * 1024;
	};

} // CW
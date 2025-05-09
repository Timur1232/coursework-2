#pragma once

#include "pch.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "engine/Object.h"
#include "engine/Chunks.h"
#include "utils/utils.h"

#include "TargetType.h"
#include "BitDirection.h"

namespace CW {

	struct BeaconSettings
	{
		float ChargeThreshold = 0.05f;
		float DischargeRate = 0.2f;
	};


	class Beacon
		: public Object
	{
	public:
		Beacon() = default;
		Beacon(sf::Vector2f position, TargetType type, uint8_t bitDirection);

		void InfoInterface(size_t index) const;

		void Update(sf::Time deltaTime, const BeaconSettings& bs);

		[[nodiscard]] bool IsAlive() const { return m_Alive; }

		void Revive(sf::Vector2f newPosition, TargetType newType, uint8_t bitDirection);

		[[nodiscard]] TargetType GetType() const { return m_Type; }
		[[nodiscard]] uint8_t GetBitDirection() const { return m_BitDirection; }
		[[nodiscard]] sf::Angle GetDirectionAngle() const;

		[[nodiscard]] sf::Color BeaconColor() const;

	private:
		TargetType m_Type = TargetType::None;
		float m_Charge = 1.0f;
		uint8_t m_BitDirection = DirectionBit::None;
		bool m_Alive = true;
	};


	class BeaconManager
		: public IUpdate
	{
	public:
		using IndexedBeacon = Indexed<Beacon>;

	public:
		BeaconManager();

		void Update(sf::Time deltaTime) override;
		void DrawAllBeacons(sf::RenderWindow& render);
		void CreateBeacon(sf::Vector2f position, TargetType type, uint8_t bitDirection);
		void Clear();

		[[nodiscard]] const ChunkHandler<Beacon>& GetChuncks() const { return m_Chunks; }
		[[nodiscard]] size_t Size() const { return m_Beacons.size(); }
		[[nodiscard]] size_t Capacity() const { return m_Beacons.capacity(); }
		
		void InfoInterface(bool* open);
		void DebugInterface();

	private:
		// FIXME: при реалокации памяти в векторе сломаются указатели в чанках
		std::vector<IndexedBeacon> m_Beacons;
		ChunkHandler<Beacon> m_Chunks{ 500.0f };
		size_t m_DeadBeacons = 0;

		BeaconSettings m_BeaconSettings;
		sf::CircleShape m_Mesh{10.0f, 4};

		static constexpr size_t BEACONS_RESERVE = 1024 * 1024;
	};

} // CW
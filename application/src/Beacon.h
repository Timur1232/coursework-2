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

	class Beacon
		: public Object,
		  public IUpdate
	{
	public:
		Beacon() = default;
		Beacon(sf::Vector2f position, TargetType type, uint8_t bitDirection);

		static void StaticInit();

		static void DebugInterface();
		void InfoInterface(size_t index, bool* open) const;

		void Update(sf::Time deltaTime) override;

		[[nodiscard]] bool IsAlive() const { return m_Alive; }

		void Revive(sf::Vector2f newPosition, TargetType newType, uint8_t bitDirection);

		[[nodiscard]] TargetType GetType() const { return m_Type; }
		[[nodiscard]] uint8_t GetBitDirection() const { return m_BitDirection; }
		[[nodiscard]] sf::Angle GetDirectionAngle() const;

		[[nodiscard]] sf::Color BeaconColor() const;

	private:
		TargetType m_Type = TargetType::None;
		uint8_t m_BitDirection = DirectionBit::None;
		float m_Charge = 1.0f;
		bool m_Alive = true;

		static float s_ChargeThreshold;
		static float s_DischargeRate;

		static sf::CircleShape s_Mesh;
	};


	class BeaconManager
		: public IUpdate
	{
	public:
		using IndexedBeacon = Indexed<std::unique_ptr<Beacon>>;

	public:
		BeaconManager() = default;

		void Update(sf::Time deltaTime) override;
		void DrawAllBeacons(sf::RenderWindow& render);
		void CreateBeacon(sf::Vector2f position, TargetType type, uint8_t bitDirection);
		void Clear();

		void SwitchShowInfo() { m_ShowInfo = !m_ShowInfo; }
		void SetShowInfo(bool showInfo) { m_ShowInfo = !showInfo; }

		const ChunkHandler<Beacon>& GetChuncks() const { return m_Chunks; }
		size_t Size() const { return m_Beacons.size(); }
		size_t Capacity() const { return m_Beacons.capacity(); }

	private:
		std::vector<IndexedBeacon> m_Beacons;
		ChunkHandler<Beacon> m_Chunks{ 500.0f };
		size_t m_DeadBeacons = 0;
		bool m_ShowInfo = false;

		sf::CircleShape m_Mesh;

		static constexpr size_t BEACONS_RESERVE = 1024 * 1024;
	};

} // CW
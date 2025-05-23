#pragma once

#include "pch.h"

#include "debug_utils/LineShape.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "engine/Object.h"
#include "engine/Events/EventInterface.h"
#include "engine/Events/UserEvents.h"
#include "Chunks.h"

#include "Beacon.h"
#include "ResourceReciever.h"
#include "Resource.h"
#include "Terrain.h"
#include "DroneSettings.h"

namespace CW {

	class Drone
		: public Object
	{
	public:
		Drone() = default;
		Drone(sf::Vector2f position, sf::Angle directionAngle = sf::Angle::Zero, TargetType target = TargetType::Recource);

		void ResetTarget() { m_TargetResourceIndex = {}; }

		[[nodiscard]] sf::Angle GetDirection() const { return m_DirectionAngle; }
		[[nodiscard]] sf::Angle GetAttraction() const { return m_AttractionAngle; }
		[[nodiscard]] float GetBeaconSpawnTimer() const { return m_BeaconTimerSec; }
		[[nodiscard]] int GetCarriedResources() const { return m_CarriedResources; }
		[[nodiscard]] std::optional<size_t> GetTargetResourceIndex() const { return m_TargetResourceIndex; }

		void SetDirection(sf::Angle angle) { m_DirectionAngle = angle; }
		void SetAttraction(sf::Angle angle) { m_AttractionAngle = angle; }

		void Update(float deltaTime, const DroneSettings& settings, std::vector<Resource>& resources);

		void ReactToBeacons(const ChunkHandler<Beacon>& beacons, float wanderCooldownSec, float FOV, sf::Vector2f viewDistance);
		[[nodiscard]] bool ReactToResourceReciver(ResourceReciever& reciever, float wanderCooldownSec);

		void ReactToResources(std::vector<Resource>& resources, sf::Vector2f viewDistance, float FOV);
		[[nodiscard]] bool CheckResourceColission(float pickUpDist, const std::vector<Resource>& resources);

	private:
		inline void turn(float deltaTime, sf::Angle turningSpeed, sf::Angle maxTurningDelta);
		inline void wander(float deltaTime, sf::Angle wanderAngleThreshold, sf::Angle randomWanderAngle, const std::vector<Resource>& resource);
		std::pair<const Beacon*, float> findFurthestInChunk(const Chunk<Beacon>* chunk, float FOV, sf::Vector2f viewDistance);

	private:
		sf::Angle m_DirectionAngle = sf::Angle::Zero;
		sf::Angle m_AttractionAngle = sf::Angle::Zero;

		TargetType m_TargetType = TargetType::Recource;
		std::optional<size_t> m_TargetResourceIndex{};

		int m_CarriedResources = 0;
		float m_BeaconTimerSec = 0.0f;
		float m_WanderTimer = 0.0f;
	};


	struct SimulationState;
	struct FullSimulationState;

	class DroneManager
		: public IOnEvent
	{
	public:
		DroneManager();
		DroneManager(const DroneSettings& settings);

		void SetState(FullSimulationState& state);

		void CollectState(SimulationState& state) const;
		void CollectState(FullSimulationState& state) const;

		void SetSettings(const DroneSettings& settings);

		void UpdateAllDrones(
			float deltaTime,
			std::vector<Resource>& resources,
			const ChunkHandler<Beacon>& beacons,
			ResourceReciever& reciever,
			const Terrain& terrain);

		void DrawAllDrones();

		void OnEvent(Event& event) override;

		void Clear();
		void Reset(size_t droneCount, sf::Vector2f startPosition = { 0.0f, 0.0f }, TargetType target = TargetType::Recource);

		void CreateDrone(sf::Vector2f position, sf::Angle directionAngle, TargetType target = TargetType::Recource);

		[[nodiscard]] size_t Size() const { return m_Drones.size(); }
		[[nodiscard]] size_t Capacity() const { return m_Drones.capacity(); }

		void InfoInterface(bool* open);
		void DebugInterface();

		void SetDefaultSettings();

		sf::Vector2f GetFurthestHorizontalReach() const { return m_FurthestHorizontalReach; }

	private:
		bool OnSpawnDrone(SpawnDrone& e);
		// Debug
		inline void debugDrawDirectionVisuals(sf::Vector2f position, sf::Angle directionAngle, sf::Angle attractionAngle) const;
		inline void debugDrawViewDistance(sf::Vector2f position, sf::Angle directionAngle) const;

	private:
		std::vector<Drone> m_Drones;
		DroneSettings m_DroneSettings;
		sf::Vector2f m_FurthestHorizontalReach;

		Unique<sf::Sprite> m_Sprite;
		Unique<sf::Texture> m_Texture;

		float m_FOVRadPrecalc;

		// Debug
		bool m_DrawViewDistance = false;
		bool m_DrawDirection = false;
	};

} // CW
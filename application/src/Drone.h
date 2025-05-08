#pragma once

#include "pch.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"
#include "engine/Object.h"
#include "engine/Chunks.h"

#include "Beacon.h"
#include "ResourceReciever.h"
#include "Resource.h"
#include "debug_utils/LineShape.h"

namespace CW {

	struct DroneSettings
	{
		float Speed;
		sf::Angle TurningSpeed;

		float FOV;
		float FOVRad;

		sf::Vector2f ViewDistance;
		float PickupDist;

		float BeaconCooldownSec;
		float WanderCooldownSec;

		sf::Angle RandomWanderAngle;
		sf::Angle WanderAngleThreshold;
		sf::Angle MaxTurningDelta;

		DroneSettings();
		void SetDefault();
	};


	class Drone
		: public Object
	{
	public:
		Drone(sf::Vector2f position, sf::Angle directionAngle = sf::Angle::Zero, TargetType target = TargetType::Recource);

		[[nodiscard]] sf::Angle GetDirection() const { return m_DirectionAngle; }
		[[nodiscard]] sf::Angle GetAttraction() const { return m_AttractionAngle; }
		[[nodiscard]] float GetBeaconSpawnTimer() const { return m_BeaconTimerSec; }
		[[nodiscard]] int GetCarriedResources() const { return m_CarriedResources; }
		[[nodiscard]] const Resource* GetTargetResource() const { return m_TargetResource; }

		void Update(sf::Time deltaTime, const DroneSettings& settings);

		void ReactToBeacons(const ChunkHandler<Beacon>& beacons, float wanderCooldownSec, float FOV, sf::Vector2f viewDistance);
		bool ReactToResourceReciver(ResourceReciever& reciever, float wanderCooldownSec);

		void ReactToResources(std::vector<std::unique_ptr<Resource>>& resources, sf::Vector2f viewDistance, float FOV);
		bool CheckResourceColission(float pickUpDist);

	private:
		inline void turn(sf::Time deltaTime, sf::Angle turningSpeed, sf::Angle maxTurningDelta);
		inline void wander(sf::Time deltaTime, sf::Angle wanderAngleThreshold, sf::Angle randomWanderAngle);
		std::pair<const Beacon*, float> findFurthestInChunk(const Chunk<Beacon>* chunk, float FOV, sf::Vector2f viewDistance);

	private:
		sf::Angle m_DirectionAngle = sf::Angle::Zero;
		sf::Angle m_AttractionAngle;

		TargetType m_TargetType = TargetType::Recource;
		Resource* m_TargetResource = nullptr;

		int m_CarriedResources = 0;
		float m_BeaconTimerSec = 0.0f;
		float m_WanderTimer = 0.0f;
	};


	class DroneManager
	{
	public:
		DroneManager() = default;

		void UpdateAllDrones(
			sf::Time deltaTime,
			std::vector<std::unique_ptr<Resource>>& resources,
			const ChunkHandler<Beacon>& beacons,
			ResourceReciever& reciever);

		void DrawAllDrones(sf::RenderWindow& render);

		void Clear();
		void Reset(size_t droneCount, sf::Vector2f startPosition = { 0.0f, 0.0f }, TargetType target = TargetType::Recource);

		void CreateDrone(sf::Vector2f position, sf::Angle directionAngle, TargetType target);

		[[nodiscard]] size_t Size() const { return m_Drones.size(); }
		[[nodiscard]] size_t Capacity() const { return m_Drones.capacity(); }

		void InfoInterface(bool* open);
		void DebugInterface();

		void SetDefaultSettings();

	private:
		// Debug
		inline void setMeshPos(sf::Vector2f position, sf::Angle directionAngle, sf::Angle attractionAngle);

	private:
		std::vector<Drone> m_Drones;
		DroneSettings m_DroneSettings;

		// Debug
		sf::CircleShape m_Mesh;
		sf::CircleShape m_MeshViewDistance;
		sf::CircleShape m_DirectionVisual;
		sf::CircleShape m_AttractionAngleVisual;
		std::array<LineShape, 2> m_FOVVisual;

		bool m_DrawViewDistance = false;
		bool m_DrawDirection = false;
	};

} // CW
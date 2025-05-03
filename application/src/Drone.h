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

	class Drone
		: public Object,
		  public IUpdate,
		  public IDrawable
	{
	public:
		Drone(sf::Vector2f position, sf::Angle directionAngle = sf::Angle::Zero, TargetType target = TargetType::Recource);

		static void StaticInit();

		static void DebugInterface();
		void InfoInterface(size_t index, bool* open) const;

		void Update(sf::Time deltaTime) override;
		void Draw(sf::RenderWindow& render) override;

		void ReactToBeacons(const ChunkHandler<Beacon>& beacons);
		bool ReactToResourceReciver(ResourceReciever& reciever);

		void ReactToResources(std::vector<Resource>& resources);
		bool CheckResourceColission();

	private:
		inline void turn(sf::Time deltaTime);
		inline void wander(sf::Time deltaTime);
		std::pair<const Beacon*, float> findFurthestInChunk(const Chunk<Beacon>* chunk);

		// Debug
		inline void setMeshPos(sf::Vector2f position) const;

	private:
		sf::Angle m_DirectionAngle = sf::Angle::Zero;
		sf::Angle m_AttractionAngle;

		TargetType m_TargetType = TargetType::Recource;
		Resource* m_TargetResource = nullptr;

		int m_CarriedResources = 0;
		float m_BeaconTimerSec = s_BeaconCooldownSec;
		float m_WanderTimer = 0.0f;

		//=================[static]=================//
		static constexpr sf::Vector2f ONE_LENGTH_VEC = { 1.0f, 0.0f };
		
		static float s_Speed;
		static sf::Angle s_TurningSpeed;
		
		static float s_FOV;
		static float s_FOVRad;

		static sf::Vector2f s_ViewDistanse;
		static float s_PickupDist;

		static float s_BeaconCooldownSec;
		static float s_WanderCooldownSec;

		static sf::Angle s_RandomWanderAngle;
		static sf::Angle s_WanderAngleThreshold;
		static sf::Angle s_MaxTurningDelta;

		// Debug
		static sf::CircleShape s_Mesh;
		static sf::CircleShape s_MeshViewDistance;
		static sf::CircleShape s_DirectionVisual;
		static sf::CircleShape s_AttractionAngleVisual;
		static std::array<LineShape, 2> s_FOVVisual;

		static bool s_DrawViewDistance;
		static bool s_DrawDirection;
	};


	class DroneManager
	{
	public:


	private:

	};

} // CW
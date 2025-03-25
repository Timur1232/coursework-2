#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "IDrawable.h"
#include "ISimulationObject.h"
#include "beacons/BeaconManager.h"

namespace CW {

	enum class DroneType
	{
		None = 0,
		Scout,
		Hauler,
		Builder
	};


	enum class DroneState
	{
		None = 0,
		Wandering,
		SearchResource,
		DeviverResource
	};


	class Drone
		: public IDrawable,
		  public ISimulationObject
	{
	public:
		Drone(glm::vec2 position)
			: m_Position(position)
		{ }

		virtual DroneType getDroneType() const { return DroneType::None; }
		virtual DroneState getDroneState() const { return m_State; }
		bool isAlive() const { return m_Alive; }

		virtual void reactToBeacons(const std::vector<Beacon*>&) = 0;
		virtual void leaveBeacon(BeaconManager&) = 0;

	protected:
		glm::vec2 m_Position{ 0.0f, 0.0f };
		glm::vec2 m_Direction{ 0.0f, 0.0f };

		DroneState m_State = DroneState::None;
		bool m_Alive = true;
	};


	class HaulerDrone
		: public Drone
	{
	public:
		HaulerDrone(glm::vec2 position)
			: Drone(position) 
		{ }

		void reactToBeacons(const std::vector<Beacon*>&) override;
		void leaveBeacon(BeaconManager&) override;

		void draw(const Window& window, const Camera2D& camera) const override;
		void update(uint64_t currentTick) override;

	private:
	};

} // CW
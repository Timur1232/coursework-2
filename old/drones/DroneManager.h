#pragma once

#include <vector>

#include "ISimulationSubsystem.h"
#include "IDrawable.h"

#include "drones/Drone.h"

namespace CW
{

	class DroneManager
		: public ISimulationSubsystem,
		  public IDrawable
	{
		// TODO
	public:
		void update(uint64_t currentTick) override;
		void draw(const Window&, const Camera2D&) const override;

	private:


	private:
		std::vector<Drone*> m_Drones;
	};

} // CW
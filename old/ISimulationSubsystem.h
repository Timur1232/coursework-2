#pragma once

#include <Window.h>

namespace CW
{

	class ISimulationSubsystem
	{
	public:
		virtual void update(uint64_t currentTick) = 0;
	};

} // CW
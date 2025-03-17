#pragma once

#include "ISimulationSubsystem.h"

namespace CW
{

	class ResourceManager
		: public ISimulationSubsystem
	{
		// TODO
	public:
		void update(uint64_t currentTick) override;
	};

} // CW
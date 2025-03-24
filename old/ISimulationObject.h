#pragma once

#include <cstdint>

namespace CW
{

	class ISimulationObject
	{
	public:
		virtual void update(uint64_t currentTick) = 0;
	};

} // CW
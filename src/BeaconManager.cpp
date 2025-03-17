#include "BeaconManager.h"

namespace CW
{

	BeaconManager::BeaconManager()
	{
		m_Beacons.reserve(1024);
	}

	void BeaconManager::update(uint64_t currentTick)
	{
		for (auto it : m_Beacons)
		{
			it->update(currentTick);
		}
	}

}
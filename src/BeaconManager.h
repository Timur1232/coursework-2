#pragma once

#include <vector>

#include "ISimulationSubsystem.h"
#include "IDrawable.h"
#include "utils/ArenaAllocator.h"
#include "beacons/Beacon.h"

namespace CW
{

	class BeaconManager
		: public ISimulationSubsystem,
		  public IDrawable
	{
	public:
		BeaconManager();

		void update(uint64_t currentTick) override;
		void draw(const Window&) const override;

		void createNavigationBeacon();
		void createRecourceBeacon();

		int getBeaconFadingTime() const { return m_BeaconFadingTime; }
		void setBeaconFadingTime(int fadingTime) { m_BeaconFadingTime = fadingTime; }

	private:


	private:
		ArenaAllocator<char, 20 * 1024> m_BeaconAllocator;
		std::vector<Beacon*> m_Beacons;
		int m_BeaconFadingTime = 3600;
	};

} // CW
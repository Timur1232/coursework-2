#pragma once

#include <vector>

#include <Window.h>
#include <Camera2D.h>
#include <drones/DroneManager.h>
#include <beacons/BeaconManager.h>
#include <Terrain.h>
#include <resources/ResourceManager.h>

namespace CW
{

	class Simulation
	{
	public:
		Simulation() = default;
		Simulation(int width, int height);

		//void initialize();
		void run();
		//void terminate();

	private:
		Window m_Window;
		Camera2D m_Camera;

		Terrain m_Terrain;
		DroneManager m_DroneManager;
		BeaconManager m_BeaconManager;
		ResourceManager m_RecourceManager;

		uint64_t m_Ticks = 0;
	};

} // CW
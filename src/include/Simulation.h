#pragma once

#include <Window.h>
#include <Camera2D.h>
#include <vector>
#include <drones/DroneBase.h>

namespace CW
{

	class Simulation
	{
	public:
		Simulation() = default;
		Simulation(int width, int height);

		void initialize();
		void run();
		void terminate();

	private:
		Window m_Window;
		Camera2D m_Camera;

		Terrain m_Terrain;

		MotherBase m_MotherBase;
		std::vector<DroneBase*> m_Drones;
		BeaconManager m_BeaconManager;
		ResourceManager m_RecourceManager;
		
	};

} // CW
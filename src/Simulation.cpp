#include "Simulation.h"

namespace CW
{

	Simulation::Simulation(int width, int height)
		: m_Window(width, height, "Simulation")
	{
	}

	void Simulation::run()
	{
		while (!m_Window.shouldClose())
		{
			m_Window.clearScreen();
			m_Window.events().pollEvents();

			m_DroneManager.update(m_Ticks);
			m_BeaconManager.update(m_Ticks);
			m_RecourceManager.update(m_Ticks);

			m_Terrain.draw(m_Window);
			m_DroneManager.draw(m_Window);
			m_BeaconManager.draw(m_Window);

			++m_Ticks;
		}
	}

} // CW
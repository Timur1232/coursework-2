#include <iostream>

#include "Window.h"
#include "camera/Camera2D.h"
#include "graphics/DebugRender2D.h"

#include "drones/Drone.h"
#include "beacons/Beacon.h"

using namespace CW;

int main()
{
    Window window(1000, 600, "Test");
	if (!window)
	{
		std::cerr << "Unable to create window" << std::endl;
		return 1;
	}

	Camera2D camera;

	DebugRender2D::initialize();

    Drone* drone = new HaulerDrone({1500.0, 0.0});

	while (!window.shouldClose())
	{
		window.events().pollEvents();
		window.clearScreen();

		drone->draw(window, camera);

		window.swapBuffers();
	}

}
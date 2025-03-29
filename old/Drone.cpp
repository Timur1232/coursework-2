#include "Drone.h"

#include "graphics/DebugRender2D.h"

namespace CW
{

	void HaulerDrone::reactToBeacons(const std::vector<Beacon*>&)
	{
	}

	void HaulerDrone::leaveBeacon(BeaconManager&)
	{
	}

	void HaulerDrone::draw(const Window& window, const Camera2D& camera) const
	{
		glm::vec2 pos = m_Position - camera.position;
		DebugRender2D::drawCircle(pos, 50, { 1.0f, 1.0f, 1.0f }, window);
	}

	void HaulerDrone::update(uint64_t currentTick)
	{
	}

} // CW
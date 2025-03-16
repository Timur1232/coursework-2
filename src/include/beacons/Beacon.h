#pragma once

#include <glm/glm.hpp>
#include <CI/ISimulationObject.h>

namespace CW
{

	class BeaconBase
		: ISimulationObject
	{
	public:
		BeaconBase(glm::vec2 position);



	private:
		glm::vec2 m_Position;
	};

} // CW
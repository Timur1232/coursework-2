#include <drones/DroneBase.h>

namespace CW
{

	/*AgentBase::AgentBase(glm::vec2 position, float speed, float signalStrength)
		: m_Position(position), m_Speed(speed), m_SignalStrength(signalStrength)
	{
	}

	void AgentBase::sendSignal(std::vector<AgentBase*> agents) const
	{
		for (auto agent : agents)
		{
			float dist = glm::distance(m_Position, agent->getPos());
			if (dist <= m_SignalStrength / m_Depth)
			{
				agent->recieveSignal(formSignal());
			}
		}
	}

	glm::vec2 AgentBase::getPos() const
	{
		return m_Position;
	}

	void AgentBase::setPos(glm::vec2 position)
	{
		m_Position = position;
	}*/

}
#pragma once

#include <glm/glm.hpp>
#include <CI/ISimulationObject.h>
#include <vector>

namespace CW
{

	/*class AgentBase
		: ISimulationObject
	{
	public:
		struct Signal
		{
			glm::vec2 position;
			glm::vec2 direction;
			uint32_t targetTypeId;
		};

	public:
		AgentBase() = default;
		AgentBase(glm::vec2 position, float speed, float signalStrength);

		void sendSignal(std::vector<AgentBase*> agents) const;
		virtual void recieveSignal(const Signal& signal) = 0;
		
		glm::vec2 getPos() const;
		void setPos(glm::vec2 position);

	protected:
		virtual Signal formSignal() const = 0;

	private:
		glm::vec2 m_Position{ 0.0f, 0.0f };
		glm::vec2 m_Direction{ 0.0f, 0.0f };
		float m_Depth = 0.0f;

		const float m_Speed = 0.0f;
		const float m_SignalStrength = 0.0f;
	};*/

	class DroneBase
		: ISimulationObject
	{
	public:
		DroneBase(glm::vec2 position);

		Beacon createBeacon();

	private:
		glm::vec2 m_Position{ 0.0f, 0.0f };
		glm::vec2 m_Direction{ 0.0f, 0.0f };
	};

} // CW
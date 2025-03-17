#pragma once

#include <glm/glm.hpp>

#include "IDrawable.h"
#include "ISimulationObject.h"

namespace CW
{

	enum class BeaconType
	{
		None = 0,
		Navigation,
		Resource
	};


	class Beacon
		: public IDrawable,
		  public ISimulationObject
	{
	public:
		Beacon(glm::vec2 position, uint64_t fadingTime)
			: m_Position(position), m_FadingTimer(fadingTime)
		{ }

		virtual BeaconType getBeaconType() const { return BeaconType::None; }
		glm::vec2 getPosition() const { return m_Position; }
		void update(uint64_t currentTick) override;
		bool isAlive() const { return m_Alive; }

	protected:
		glm::vec2 m_Position;
		uint64_t m_FadingTimer;
		bool m_Alive = true;
	};


	class NavigationBeacon
		: Beacon
	{
	public:
		NavigationBeacon(glm::vec2 position, uint64_t fadingTime)
			: Beacon(position, fadingTime)
		{ }

		BeaconType getBeaconType() const override { return BeaconType::Navigation; }
		void draw(const Window& window) const override;
	};


	class ResourceBeacon
		: Beacon
	{
	public:
		ResourceBeacon(glm::vec2 position, uint64_t fadingTime)
			: Beacon(position, fadingTime)
		{ }

		BeaconType getBeaconType() const override { return BeaconType::Resource; }
		void draw(const Window& window) const override;
	};

} // CW
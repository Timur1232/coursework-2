#pragma once
#include "pch.h"

#include "Event.h"
#include "TargetType.h"

#include "SimulationSettings.h"

namespace CW {

	class CreateBeacon
		: public Event
	{
	public:
		sf::Vector2f Position;
		CW::TargetType Type;
		uint8_t BitDirection;

		CreateBeacon(sf::Vector2f position, CW::TargetType type, uint8_t bitDirection)
			: Position(position), Type(type), BitDirection(bitDirection)
		{
		}
		EventType GetEventType() const { return EventType::CreateBeacon; }
		static EventType GetStaticEventType() { return EventType::CreateBeacon; }
	};

	class CloseApp
		: public Event
	{
	public:
		CloseApp() = default;
		EventType GetEventType() const { return EventType::CloseApp; }
		static EventType GetStaticEventType() { return EventType::CloseApp; }
	};

	class StartSimulation
		: public Event
	{
	public:
		const SimulationSettings* Settings;

		StartSimulation(const SimulationSettings* settings)
			: Settings(settings)
		{
		}
		EventType GetEventType() const { return EventType::StartSimulation; }
		static EventType GetStaticEventType() { return EventType::StartSimulation; }
	};

	class SetSimulationSettings
		: public Event
	{
	public:
		const SimulationSettings* Settings;

		SetSimulationSettings(const SimulationSettings* settings)
			: Settings(settings)
		{
		}
		EventType GetEventType() const { return EventType::SetSimulationSettings; }
		static EventType GetStaticEventType() { return EventType::SetSimulationSettings; }
	};

} // CW
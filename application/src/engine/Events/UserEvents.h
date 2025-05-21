#pragma once
#include "pch.h"

#include "Event.h"
#include "TargetType.h"

#include "SimulationSettings.h"

#define CW_BULD_EVENT_TYPE(type) EventType GetEventType() const { return EventType::type; } \
static EventType GetStaticEventType() { return EventType::type; }

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
		CW_BULD_EVENT_TYPE(CreateBeacon)
	};

	class CloseApp
		: public Event
	{
	public:
		CloseApp() = default;
		CW_BULD_EVENT_TYPE(CloseApp)
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
		CW_BULD_EVENT_TYPE(StartSimulation)
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
		CW_BULD_EVENT_TYPE(SetSimulationSettings)
	};

	class SpawnDrone
		: public Event
	{
	public:
		sf::Vector2f Position;

		SpawnDrone(sf::Vector2f position)
			: Position(position)
		{
		}
		CW_BULD_EVENT_TYPE(SpawnDrone)
	};

	class SwitchMenu
		: public Event
	{
	public:
		SwitchMenu() = default;
		CW_BULD_EVENT_TYPE(SwitchMenu)
	};

	class SwitchDebugMenu
		: public Event
	{
	public:
		SwitchDebugMenu() = default;
		CW_BULD_EVENT_TYPE(SwitchDebugMenu)
	};

} // CW
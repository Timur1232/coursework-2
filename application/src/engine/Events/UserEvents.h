#pragma once
#include "pch.h"

#include "Event.h"
#include "TargetType.h"

#include "SimulationSettings.h"

#define CW_BULD_EVENT_TYPE(type) EventType GetEventType() const { return EventType::type; } \
static EventType GetStaticEventType() { return EventType::type; }

namespace CW {

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
		StartSimulation() = default;
		CW_BULD_EVENT_TYPE(StartSimulation)
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

	class LoadSimulation
		: public Event
	{
	public:
		std::string FilePath;

		LoadSimulation(std::string_view path) : FilePath(path) {}
		CW_BULD_EVENT_TYPE(LoadSimulation)
	};

	class MessegeToUser
		: public Event
	{
	public:
		std::string Messege;

		MessegeToUser(const char* messege) : Messege(messege) {}
		CW_BULD_EVENT_TYPE(MessegeToUser)
	};

	class SimulationOver
		: public Event
	{
	public:
		SimulationOver() = default;
		CW_BULD_EVENT_TYPE(SimulationOver)
	};

} // CW
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

	class SaveSimulation
		: public Event
	{
	public:
		const char* FilePath;

		SaveSimulation(const char* path) : FilePath(path) {}
		CW_BULD_EVENT_TYPE(SaveSimulation)
	};

	class LoadSimulation
		: public Event
	{
	public:
		std::string FilePath;

		LoadSimulation(std::string_view path) : FilePath(path) {}
		CW_BULD_EVENT_TYPE(LoadSimulation)
	};

	class SwitchThread
		: public Event
	{
	public:
		int TargetUps;

		SwitchThread(int targetUPS) : TargetUps(targetUPS) {}
		CW_BULD_EVENT_TYPE(SwitchThread)
	};

} // CW
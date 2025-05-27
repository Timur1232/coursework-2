#include "pch.h"
#include "UserEventHandler.h"

namespace CW {

	EventHandler& EventHandler::Get()
	{
		static EventHandler handler;
		return handler;
	}

	void EventHandler::Reserve(size_t eventReserve)
	{
		m_UserEvents.reserve(eventReserve);
	}

} // CW
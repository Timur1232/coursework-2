#include "pch.h"
#include "UserEventHandler.h"

namespace CW {

	UserEventHandler& UserEventHandler::Get()
	{
		static UserEventHandler handler;
		return handler;
	}

	void UserEventHandler::Reserve(size_t eventReserve)
	{
		m_UserEvents.reserve(eventReserve);
	}

} // CW
#pragma once

#include "pch.h"

namespace CW {

	class Event;

	class IOnEvent
	{
	public:
		virtual ~IOnEvent() = default;
		virtual bool OnEvent(Event& event) = 0;
	};

} // CW
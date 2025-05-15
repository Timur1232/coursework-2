#pragma once

#include "pch.h"

namespace CW {

	class Event;

	class IOnEvent
	{
	public:
		virtual ~IOnEvent() = default;
		virtual void OnEvent(Event& event) = 0;
	};

} // CW
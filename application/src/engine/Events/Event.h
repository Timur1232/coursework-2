#pragma once

#include "pch.h"
#include "CW_EventConfig.h"

namespace CW {

	enum class EventType
	{
		None = 0,
		Closed, Resized,
		KeyPressed, KeyReleased,
		MouseWheelScrolled, MouseButtonPressed, MouseButtonReleased, MouseMoved,
		CW_USER_EVENTS_LIST
	};

	class Event
	{
	public:
		bool Handled = false;

		virtual ~Event() = default;
		virtual EventType GetEventType() const = 0;
	};

	template <class T>
	concept has_static_event_type = requires()
	{
		{ T::GetStaticEventType() } -> std::same_as<EventType>;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		template <std::derived_from<Event> T, class F>
		bool Dispach(const F& func)
		{
			if (m_Event.GetEventType() == T::GetStaticEventType())
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

} // CW

#define CW_BUILD_EVENT_FUNC(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }
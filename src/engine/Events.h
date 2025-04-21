#pragma once

#include "pch.h"

#include "utils/utils.h"
#include "debug_utils/Log.h"

#include "EventInterface.h"

#include "CW_EventConfig.h"
#include "UserEvent.h"

namespace CW {

	class KeyPressedObs
		: virtual public OnEvent
	{
	public:
		~KeyPressedObs() = default;
		virtual void OnKeyPressed(const sf::Event::KeyPressed*) = 0;
	};

	class KeyReleasedObs
		: virtual public OnEvent
	{
	public:
		virtual ~KeyReleasedObs() = default;
		virtual void OnKeyReleased(const sf::Event::KeyReleased*) = 0;
	};

	class MouseButtonPressedObs
		: virtual public OnEvent
	{
	public:
		virtual ~MouseButtonPressedObs() = default;
		virtual void OnMouseButtonPressed(const sf::Event::MouseButtonPressed*) = 0;
	};

	class MouseButtonReleasedObs
		: virtual public OnEvent
	{
	public:
		virtual ~MouseButtonReleasedObs() = default;
		virtual void OnMouseButtonReleased(const sf::Event::MouseButtonReleased*) = 0;
	};

	class MouseMovedObs
		: virtual public OnEvent
	{
	public:
		virtual ~MouseMovedObs() = default;
		virtual void OnMouseMoved(const sf::Event::MouseMoved*) = 0;
	};

	class MouseWheelScrolledObs
		: virtual public OnEvent
	{
	public:
		virtual ~MouseWheelScrolledObs() = default;
		virtual void OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled*) = 0;
	};

	class ClosedObs
		: virtual public OnEvent
	{
	public:
		virtual ~ClosedObs() = default;
		virtual void OnClosed() = 0;
	};

	class ResizedObs
		: virtual public OnEvent
	{
	public:
		virtual ~ResizedObs() = default;
		virtual void OnResized(const sf::Event::Resized*) = 0;
	};


	template<class Target_t, class Event_t>
	class CoreDispatcher
	{
	public:
		CoreDispatcher(OnEvent* target, const sf::Event& event)
			: m_Target(dynamic_cast<Target_t*>(target)), m_Event(event.getIf<Event_t>())
		{
		}

		bool IsValid() const
		{
			bool valid = m_Target && m_Event;
			return valid;
		}

		void operator()()
		{
			CW_ASSERT("Unknown event subscriber type!");
		}

	private:
		Target_t* m_Target = nullptr;
		const Event_t* m_Event = nullptr;
	};

	class ProgramCore;

	class EventHandler
	{
	public:
		EventHandler(const EventHandler&) = delete;
		EventHandler(EventHandler&&) = delete;

		static EventHandler& Get();

		void Reserve(size_t targetReserve, size_t eventReserve);

		size_t Subscribe(OnEvent* target);
		void Unsubscribe(size_t index);
		void HandleEvents(sf::RenderWindow& window);

#ifdef CW_USER_EVENTS_LIST

		void HandleUserEvents();

		template<class T>
		void AddEvent(T&& event)
		{
			m_UserEvents.push_back(MyEvent{ std::move(event) });
		}

#endif

	private:
		EventHandler() = default;

	private:
		std::vector<OnEvent*> m_EventTargets;
#ifdef CW_USER_EVENTS_LIST
		std::vector<MyEvent> m_UserEvents;
#endif
		size_t m_UnsubsCount = 0;
	};


	/*class EventHandlerWrapper
	{
	public:
		EventHandlerWrapper() = default;
		EventHandlerWrapper(EventHandler* handler)
			: m_EventHandler(handler)
		{
		}

		size_t subscribe(OnEvent* target);
		void unsubscribe(size_t index);

#ifdef CW_USER_EVENTS_LIST
		template<class T>
		void addEvent(T&& event)
		{
			m_EventHandler->addEvent<T>(std::forward<T>(event));
		}
#endif

	private:
		EventHandler* m_EventHandler = nullptr;
	};	*/

} // CW
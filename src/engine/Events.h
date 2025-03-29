#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "utils/utils.h"
#include "debug_utils/Log.h"

#include "EventInterface.h"

#include "CW_EventConfig.h"
#include "UserEvent.h"

namespace CW_E {

	class OnKeyPressed
		: virtual public OnEvent
	{
	public:
		~OnKeyPressed() = default;
		virtual void onKeyPressed(const sf::Event::KeyPressed*) = 0;
	};

	class OnKeyReleased
		: virtual public OnEvent
	{
	public:
		virtual ~OnKeyReleased() = default;
		virtual void onKeyReleased(const sf::Event::KeyReleased*) = 0;
	};

	class OnMouseButtonPressed
		: virtual public OnEvent
	{
	public:
		virtual ~OnMouseButtonPressed() = default;
		virtual void onMouseButtonPressed(const sf::Event::MouseButtonPressed*) = 0;
	};

	class OnMouseButtonReleased
		: virtual public OnEvent
	{
	public:
		virtual ~OnMouseButtonReleased() = default;
		virtual void onMouseButtonReleased(const sf::Event::MouseButtonReleased*) = 0;
	};

	class OnMouseMoved
		: virtual public OnEvent
	{
	public:
		virtual ~OnMouseMoved() = default;
		virtual void onMouseMoved(const sf::Event::MouseMoved*) = 0;
	};

	class OnMouseWheelScrolled
		: virtual public OnEvent
	{
	public:
		virtual ~OnMouseWheelScrolled() = default;
		virtual void onMouseWheelScrolled(const sf::Event::MouseWheelScrolled*) = 0;
	};

	class OnClosed
		: virtual public OnEvent
	{
	public:
		virtual ~OnClosed() = default;
		virtual void onClosed() = 0;
	};

	class OnResized
		: virtual public OnEvent
	{
	public:
		virtual ~OnResized() = default;
		virtual void onResized(const sf::Event::Resized*) = 0;
	};


	template<class Target_t, class Event_t>
	class CoreDispatcher
	{
	public:
		CoreDispatcher(OnEvent* target, const sf::Event& event)
			: target(dynamic_cast<Target_t*>(target)), event(event.getIf<Event_t>())
		{
		}

		bool isValid() const
		{
			bool valid = target && event;
			if (valid) valid = target->isAcceptingEvents();
			return valid;
		}

		void operator()()
		{
			CW_ASSERT("Unknown event subscriber type!");
		}

	private:
		Target_t* target = nullptr;
		const Event_t* event = nullptr;
	};

	class ProgramCore;

	class EventHandler
	{
	public:
		EventHandler() = delete;
		EventHandler(size_t reserve, size_t eventReserve);

		size_t subscribe(OnEvent* target);
		void unsubscribe(size_t index);
		void handleEvents(sf::RenderWindow& window);

#ifdef CW_USER_EVENTS_LIST

		void handleUserEvents();

		template<class T>
		void addEvent(T&& event)
		{
			m_UserEvents.push_back(MyEvent{ std::move(event) });
		}

#endif

	private:
		std::vector<OnEvent*> m_EventTargets;
#ifdef CW_USER_EVENTS_LIST
		std::vector<MyEvent> m_UserEvents;
#endif
		bool m_HasUnsubs = false;
	};


	class EventHandlerWrapper
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
	};	

} // CW_E
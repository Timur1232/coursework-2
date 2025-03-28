#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "utils/utils.h"
#include "debug_utils/Log.h"

namespace CW {

	class OnEvent
	{
	public:
		virtual ~OnEvent() = default;
		virtual bool isAcceptingEvents() const { return true; }
	};


	class OnKeyPressed
		: virtual public OnEvent
	{
	public:
		virtual void onKeyPressed(sf::Event::KeyPressed) = 0;
	};

	class OnKeyReleased
		: virtual public OnEvent
	{
	public:
		virtual void onKeyReleased(sf::Event::KeyReleased) = 0;
	};

	class OnMouseButtonPressed
		: virtual public OnEvent
	{
	public:
		virtual void onMouseButtonPressed(sf::Event::MouseButtonPressed) = 0;
	};

	class OnMouseButtonReleased
		: virtual public OnEvent
	{
	public:
		virtual void onMouseButtonReleased(sf::Event::MouseButtonReleased) = 0;
	};

	class OnMouseMoved
		: virtual public OnEvent
	{
	public:
		virtual void onMouseMoved(sf::Event::MouseMoved) = 0;
	};

	class OnMouseWheelScrolled
		: virtual public OnEvent
	{
	public:
		virtual void onMouseWheelScrolled(sf::Event::MouseWheelScrolled) = 0;
	};

	class OnClosed
		: virtual public OnEvent
	{
	public:
		virtual void onClosed() = 0;
	};

	class OnResized
		: virtual public OnEvent
	{
	public:
		virtual void onResized(sf::Event::Resized) = 0;
	};


	template<class Target_t, class Event_t>
	class Dispatcher
	{
	public:
		Dispatcher(OnEvent* target, const sf::Event& event)
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


	class EventHandler
	{
	public:
		EventHandler() = default;
		EventHandler(size_t reserve);

		void subscribe(OnEvent* newSubscriber);
		void handleEvents(sf::RenderWindow& window);

	private:
		std::vector<OnEvent*> m_EventSubscribers;
	};


	class EventHandlerWrapper
	{
	public:
		EventHandlerWrapper() = default;
		EventHandlerWrapper(EventHandler* handler)
			: m_EventHandler(handler)
		{
		}

		void subscribe(OnEvent* newSubscriber);
		operator bool() const;

	private:
		EventHandler* m_EventHandler = nullptr;
	};	

} // CW
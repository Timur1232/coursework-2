#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "utils/utils.h"

namespace CW {

	class EventHandler;
	class EventHandlerWrapper;


	class OnEvent
	{
	public:
		virtual bool isAcceptingEvents() const { return true; }
	};


	template<class Target_t, class Event_t>
	struct Dispatcher
	{
		Target_t* target = nullptr;
		const Event_t* event = nullptr;

		Dispatcher(OnEvent* target, const sf::Event& event)
			: target(dynamic_cast<Target_t*>(target)), event(event.getIf<Event_t>())
		{
		}

		operator bool() const
		{
			bool valid = target && event;
			if (valid) valid = target->isAcceptingEvents();
			return valid;
		}
	};


	class EventHandler
	{
	public:
		EventHandler() = default;
		EventHandler(size_t reserve);

		void subscribe(OnEvent* newSubscriber);
		void handleEvents(sf::RenderWindow& window);

	private:
		void dispatchEvent(OnEvent* subscriber, const sf::Event& event);

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

} // CW
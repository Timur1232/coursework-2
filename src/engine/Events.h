#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "utils/utils.h"

namespace CW {

	class EventHandler;
	class EventHandlerWrapper;	

	enum EventType : uint8_t
	{
		KeyPressed = bit(0),
		KeyReleased = bit(1),
		MouseWheelScrolled = bit(2),
		MouseButtonPressed = bit(3),
		MouseButtonReleased = bit(4),
		MouseMoved = bit(5),
		Closed = bit(6),
		Resized = bit(7),
	};


	class IOnEvent
	{
	public:
		virtual void eventSubscription(EventHandlerWrapper handler) = 0;
	};


	struct EventSubscriber
	{
		EventSubscriber(IOnEvent* target, uint8_t type)
			: target(target), type(type)
		{
		}

		IOnEvent* target;
		uint8_t type;
	};


	class EventHandler
	{
	public:
		EventHandler() = default;
		EventHandler(size_t reserve);

		void subscribe(IOnEvent* newSubscriber, uint8_t type);
		void handleEvents(sf::RenderWindow& window);

	private:
		void dispatchEvent(EventSubscriber& subscriber, const sf::Event& event);

	private:
		std::vector<EventSubscriber> m_EventSubscribers;
	};


	class EventHandlerWrapper
	{
	public:
		EventHandlerWrapper(EventHandler& handler)
			: m_EventHandler(handler)
		{
		}

		void subscribe(IOnEvent* newSubscriber, uint8_t type);

	private:
		EventHandler& m_EventHandler;
	};


	class IOnKeyPressed
		: virtual public IOnEvent
	{
	public:
		virtual void onKeyPressed(sf::Event::KeyPressed) = 0;
		virtual void eventSubscription(EventHandlerWrapper handler) override { handler.subscribe(this, EventType::KeyPressed); }
	};

	class IOnKeyReleased
		: virtual public IOnEvent
	{
	public:
		virtual void onKeyReleased(sf::Event::KeyReleased) = 0;
		virtual void eventSubscription(EventHandlerWrapper handler) override { handler.subscribe(this, EventType::KeyReleased); }
	};

	class IOnMouseButtonPressed
		: virtual public IOnEvent
	{
	public:
		virtual void onMouseButtonPressed(sf::Event::MouseButtonPressed) = 0;
		virtual void eventSubscription(EventHandlerWrapper handler) override { handler.subscribe(this, EventType::MouseButtonPressed); }
	};

	class IOnMouseButtonReleased
		: virtual public IOnEvent
	{
	public:
		virtual void onMouseButtonReleased(sf::Event::MouseButtonReleased) = 0;
		virtual void eventSubscription(EventHandlerWrapper handler) override { handler.subscribe(this, EventType::MouseButtonReleased); }
	};

	class IOnMouseMoved
		: virtual public IOnEvent
	{
	public:
		virtual void onMouseMoved(sf::Event::MouseMoved) = 0;
		virtual void eventSubscription(EventHandlerWrapper handler) override { handler.subscribe(this, EventType::MouseMoved); }
	};

	class IOnMouseWheelScrolled
		: virtual public IOnEvent
	{
	public:
		virtual void onMouseWheelScrolled(sf::Event::MouseWheelScrolled) = 0;
		virtual void eventSubscription(EventHandlerWrapper handler) override { handler.subscribe(this, EventType::MouseWheelScrolled); }
	};

	class IOnClosed
		: virtual public IOnEvent
	{
	public:
		virtual void onClosed() = 0;
		virtual void eventSubscription(EventHandlerWrapper handler) override { handler.subscribe(this, EventType::Closed); }
	};

	class IOnResized
		: virtual public IOnEvent
	{
	public:
		virtual void onResized(sf::Event::Resized) = 0;
		virtual void eventSubscription(EventHandlerWrapper handler) override { handler.subscribe(this, EventType::Resized); }
	};	

} // CW
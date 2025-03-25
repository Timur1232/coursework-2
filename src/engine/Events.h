#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

namespace CW {

	class EventHandler;
	class EventHandlerWrapper;	

	class IOnEvent
	{
	public:
		virtual void onEvent(const sf::Event& event) = 0;
		virtual void eventSubscribtion(EventHandlerWrapper handler) = 0;
		/*
		virtual void onKeyPressed(sf::Event::KeyPressed) {};
		virtual void onKeyReleased(sf::Event::KeyReleased) {};

		virtual void onMouseButtonPressed(sf::Event::MouseButtonPressed) {};
		virtual void onMouseButtonReleased(sf::Event::MouseButtonReleased) {};
		virtual void onMouseWheelScrolled(sf::Event::MouseWheelScrolled) {};

		virtual void onClosed(sf::Event::Closed) {};
		virtual void onResized(sf::Event::Resized) {};
		*/
	};

	class EventHandler
	{
	public:
		EventHandler() = default;
		EventHandler(size_t reserve);

		void subscribe(IOnEvent* newSubscriber);
		void handleEvents(sf::RenderWindow& window);

	private:
		std::vector<IOnEvent*> m_EventSubscribers;
	};

	class EventHandlerWrapper
	{
	public:
		EventHandlerWrapper(EventHandler& handler)
			: m_EventHandler(handler)
		{
		}

		void subscribe(IOnEvent* newSubscriber)
		{
			m_EventHandler.subscribe(newSubscriber);
		}

	private:
		EventHandler& m_EventHandler;
	};

	// TODO: идея разделить ивенты

	/*class IOnKeyPressed
	{
	public:
		virtual void onKeyPressed(sf::Event::KeyPressed) {};
	};

	class IOnKeyReleased
	{
	public:
		virtual void onKeyReleased(sf::Event::KeyReleased) {};
	};

	class IOnMouseButtonPressed
	{
	public:
		virtual void onMouseButtonPressed(sf::Event::MouseButtonPressed) {};
	};

	class IOnMouseButtonReleased
	{
	public:
		virtual void onMouseButtonReleased(sf::Event::MouseButtonReleased) {};
	};

	class IOnMouseMoved
	{
	public:
		virtual void onMouseMoved(sf::Event::MouseMoved) {};
	};

	class IOnMouseWheelScrolled
	{
	public:
		virtual void onMouseWheelScrolled(sf::Event::MouseWheelScrolled) {};
	};

	class IOnClosed
	{
	public:
		virtual void onClosed(sf::Event::Closed) {};
	};

	class IOnResized
	{
	public:
		virtual void onResized(sf::Event::Resized) {};
	};

	template<class T, class... Args>
	class EventDispatch {};

	template<class T, class Event, class... Args>
	class EventDispatch
		: public EventDispatch<T, Args...>
	{
	public:
		EventDispatch();

		void dispatch(T* eventReciever, Event event)
		{
			dispatch()
		}
	};

	template<class T, class... Args>
	class EventDispatch
	{
	public:

	};*/

} // CW
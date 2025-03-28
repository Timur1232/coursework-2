#include "Events.h"

#include <imgui-SFML.h>

#include "debug_utils/Log.h"

namespace CW {

	template<>
	class Dispatcher<void, void>
	{
	};


	template<>
	void Dispatcher<OnKeyPressed, sf::Event::KeyPressed>::operator()()
	{
		target->onKeyPressed(*event);
	}


	template<>
	void Dispatcher<OnKeyReleased, sf::Event::KeyReleased>::operator()()
	{
		target->onKeyReleased(*event);
	}


	template<>
	void Dispatcher<OnMouseButtonPressed, sf::Event::MouseButtonPressed>::operator()()
	{
		target->onMouseButtonPressed(*event);
	}


	template<>
	void Dispatcher<OnMouseButtonReleased, sf::Event::MouseButtonReleased>::operator()()
	{
		target->onMouseButtonReleased(*event);
	}


	template<>
	void Dispatcher<OnMouseMoved, sf::Event::MouseMoved>::operator()()
	{
		target->onMouseMoved(*event);
	}


	template<>
	void Dispatcher<OnMouseWheelScrolled, sf::Event::MouseWheelScrolled>::operator()()
	{
		target->onMouseWheelScrolled(*event);
	}


	template<>
	void Dispatcher<OnClosed, sf::Event::Closed>::operator()()
	{
		target->onClosed();
	}


	template<>
	void Dispatcher<OnResized, sf::Event::Resized>::operator()()
	{
		target->onResized(*event);
	}


	template<class _EventSub = void, class _EventType = void, class... _EventSubArgs>
	void dispatchEvent(OnEvent* target, const sf::Event& event)
	{
		if (auto d = Dispatcher<_EventSub, _EventType>(target, event); d.isValid())
			d();
		dispatchEvent<_EventSubArgs...>(target, event);
	}

	template<>
	void dispatchEvent<void, void>(OnEvent*, const sf::Event&)
	{
	}

	EventHandler::EventHandler(size_t reserve)
	{
		m_EventSubscribers.reserve(reserve);
	}

	void EventHandler::subscribe(OnEvent* newSubscriber)
	{
		m_EventSubscribers.push_back(newSubscriber);
	}

	void EventHandler::handleEvents(sf::RenderWindow& window)
	{
		while (const std::optional event = window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(window, *event);

			for (auto& sub : m_EventSubscribers)
			{
				dispatchEvent<
					OnKeyPressed, sf::Event::KeyPressed,
					OnKeyReleased, sf::Event::KeyReleased,
					OnMouseButtonPressed, sf::Event::MouseButtonPressed,
					OnMouseButtonReleased, sf::Event::MouseButtonReleased,
					OnMouseMoved, sf::Event::MouseMoved,
					OnMouseWheelScrolled, sf::Event::MouseWheelScrolled,
					OnClosed, sf::Event::Closed,
					OnResized, sf::Event::Resized
				>(sub, *event);
			}
		}
	}

	/*void EventHandler::dispatchEvent(OnEvent* target, const sf::Event& event)
	{
		if (auto d = Dispatcher<OnKeyPressed, sf::Event::KeyPressed>(target, event))
			d.target->onKeyPressed(*d.event);

		if (auto d = Dispatcher<OnKeyReleased, sf::Event::KeyReleased>(target, event))
			d.target->onKeyReleased(*d.event);

		if (auto d = Dispatcher<OnMouseWheelScrolled, sf::Event::MouseWheelScrolled>(target, event))
			d.target->onMouseWheelScrolled(*d.event);

		if (auto d = Dispatcher<OnMouseButtonPressed, sf::Event::MouseButtonPressed>(target, event))
			d.target->onMouseButtonPressed(*d.event);

		if (auto d = Dispatcher<OnMouseButtonReleased, sf::Event::MouseButtonReleased>(target, event))
			d.target->onMouseButtonReleased(*d.event);

		if (auto d = Dispatcher<OnMouseMoved, sf::Event::MouseMoved>(target, event))
			d.target->onMouseMoved(*d.event);

		if (auto d = Dispatcher<OnClosed, sf::Event::Closed>(target, event))
			d.target->onClosed();

		if (auto d = Dispatcher<OnResized, sf::Event::Resized>(target, event))
			d.target->onResized(*d.event);
	}*/

	void EventHandlerWrapper::subscribe(OnEvent* newSubscriber)
	{
		m_EventHandler->subscribe(newSubscriber);
	}

	EventHandlerWrapper::operator bool() const
	{
		return m_EventHandler;
	}

} // CW
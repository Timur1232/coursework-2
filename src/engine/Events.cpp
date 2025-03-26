#include "Events.h"

#include <imgui-SFML.h>

#include "debug/Log.h"

namespace CW {

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
				dispatchEvent(sub, *event);
			}
		}
	}

	void EventHandler::dispatchEvent(OnEvent* target, const sf::Event& event)
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
	}

	void EventHandlerWrapper::subscribe(OnEvent* newSubscriber)
	{
		m_EventHandler->subscribe(newSubscriber);
	}

	EventHandlerWrapper::operator bool() const
	{
		return m_EventHandler;
	}

} // CW
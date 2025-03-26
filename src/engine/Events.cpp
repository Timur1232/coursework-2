#include "Events.h"

#include <imgui-SFML.h>

#include "debug/Log.h"

namespace CW {

	EventHandler::EventHandler(size_t reserve)
	{
		m_EventSubscribers.reserve(reserve);
	}

	void EventHandler::subscribe(IOnEvent* newSubscriber, uint8_t type)
	{
		CW_TRACE("Traced memory adress: {}", (void*)newSubscriber);
		m_EventSubscribers.emplace_back(newSubscriber, type);
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

	void EventHandler::dispatchEvent(EventSubscriber& subscriber, const sf::Event& event)
	{
		if (const auto e = event.getIf<sf::Event::KeyPressed>();
			e && (subscriber.type & EventType::KeyPressed))
		{
			CW_TRACE("KeyPressed Event dispatched.");
			CW_TRACE("Cast object IOnEvent* to IOneKeyPressed* with memory adress: {}", (void*)subscriber.target);
			dynamic_cast<IOnKeyPressed*>(subscriber.target)->onKeyPressed(*e);
		}

		else if (const auto e = event.getIf<sf::Event::KeyReleased>();
			e && (subscriber.type & EventType::KeyReleased))
		{
			dynamic_cast<IOnKeyReleased*>(subscriber.target)->onKeyReleased(*e);
		}

		else if (const auto e = event.getIf<sf::Event::MouseWheelScrolled>();
			e && (subscriber.type & EventType::MouseWheelScrolled))
		{
			dynamic_cast<IOnMouseWheelScrolled*>(subscriber.target)->onMouseWheelScrolled(*e);
		}

		else if (const auto e = event.getIf<sf::Event::MouseButtonPressed>();
			e && (subscriber.type & EventType::MouseButtonPressed))
		{
			dynamic_cast<IOnMouseButtonPressed*>(subscriber.target)->onMouseButtonPressed(*e);
		}

		else if (const auto e = event.getIf<sf::Event::MouseButtonReleased>();
			e && (subscriber.type & EventType::MouseButtonReleased))
		{
			dynamic_cast<IOnMouseButtonReleased*>(subscriber.target)->onMouseButtonReleased(*e);
		}

		else if (const auto e = event.getIf<sf::Event::MouseMoved>();
			e && (subscriber.type & EventType::MouseMoved))
		{
			dynamic_cast<IOnMouseMoved*>(subscriber.target)->onMouseMoved(*e);
		}

		else if (const auto e = event.getIf<sf::Event::Closed>();
			e && (subscriber.type & EventType::Closed))
		{
			CW_TRACE("Recieved Closed event.");
			dynamic_cast<IOnClosed*>(subscriber.target)->onClosed();
		}

		else if (const auto e = event.getIf<sf::Event::Resized>();
			e && (subscriber.type & EventType::Resized))
		{
			dynamic_cast<IOnResized*>(subscriber.target)->onResized(*e);
		}
	}

	void EventHandlerWrapper::subscribe(IOnEvent* newSubscriber, uint8_t type)
	{
		CW_TRACE("Traced memory adress: {}", (void*)newSubscriber);
		m_EventHandler.subscribe(newSubscriber, type);
	}

} // CW
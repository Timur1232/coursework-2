#include "Events.h"

#include <imgui-SFML.h>

#include "debug_utils/Log.h"

namespace CW {

	template<>
	class CoreDispatcher<void, void>
	{
	};


	template<>
	void CoreDispatcher<OnKeyPressed, sf::Event::KeyPressed>::operator()()
	{
		target->onKeyPressed(event);
	}


	template<>
	void CoreDispatcher<OnKeyReleased, sf::Event::KeyReleased>::operator()()
	{
		target->onKeyReleased(event);
	}


	template<>
	void CoreDispatcher<OnMouseButtonPressed, sf::Event::MouseButtonPressed>::operator()()
	{
		target->onMouseButtonPressed(event);
	}


	template<>
	void CoreDispatcher<OnMouseButtonReleased, sf::Event::MouseButtonReleased>::operator()()
	{
		target->onMouseButtonReleased(event);
	}


	template<>
	void CoreDispatcher<OnMouseMoved, sf::Event::MouseMoved>::operator()()
	{
		target->onMouseMoved(event);
	}


	template<>
	void CoreDispatcher<OnMouseWheelScrolled, sf::Event::MouseWheelScrolled>::operator()()
	{
		target->onMouseWheelScrolled(event);
	}


	template<>
	void CoreDispatcher<OnClosed, sf::Event::Closed>::operator()()
	{
		target->onClosed();
	}


	template<>
	void CoreDispatcher<OnResized, sf::Event::Resized>::operator()()
	{
		target->onResized(event);
	}


	template<class _EventSub = void, class _EventType = void, class... _EventSubArgs>
	void dispatchCoreEvent(OnEvent* target, const sf::Event& event)
	{
		if (auto d = CoreDispatcher<_EventSub, _EventType>(target, event); d.isValid())
		{
			d();
			return;
		}
		dispatchCoreEvent<_EventSubArgs...>(target, event);
	}

	template<>
	void dispatchCoreEvent<void, void>(OnEvent*, const sf::Event&)
	{
	}

	EventHandler::EventHandler(size_t reserve)
	{
		m_EventTargets.reserve(reserve);
	}

	void EventHandler::subscribe(OnEvent* newSubscriber)
	{
		m_EventTargets.push_back(newSubscriber);
	}

	void EventHandler::handleEvents(sf::RenderWindow& window)
	{
		while (const std::optional event = window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(window, *event);

			for (auto target : m_EventTargets)
			{
				dispatchCoreEvent<
					OnKeyPressed, sf::Event::KeyPressed,
					OnKeyReleased, sf::Event::KeyReleased,
					OnMouseButtonPressed, sf::Event::MouseButtonPressed,
					OnMouseButtonReleased, sf::Event::MouseButtonReleased,
					OnMouseMoved, sf::Event::MouseMoved,
					OnMouseWheelScrolled, sf::Event::MouseWheelScrolled,
					OnClosed, sf::Event::Closed,
					OnResized, sf::Event::Resized
				>(target, *event);
			}
		}
	}


#ifdef CW_USER_EVENTS_LIST

	template<>
	class UserDispatcher<void, void>
	{
	};

#include "CW_EventConfig-dispatcher.h"

	template<class _EventSub = void, class _EventType = void, class... _EventSubArgs>
	void dispatchUserEvent(OnEvent* target, const MyEvent& event)
	{
		if (auto d = UserDispatcher<_EventSub, _EventType>(target, event); d.isValid())
		{
			d();
			return;
		}
		dispatchUserEvent<_EventSubArgs...>(target, event);
	}

	template<>
	void dispatchUserEvent<void, void>(OnEvent*, const MyEvent&)
	{
	}


	void EventHandler::handleUserEvents()
	{
		for (auto event : m_UserEvents)
		{
			for (auto target : m_EventTargets)
			{
				dispatchUserEvent<CW_USER_EVENTS_PAIRS>(target, *event);
			}
		}
		m_UserEvents.clear();
		m_Allocator.deallocate();
	}

#endif


	void EventHandlerWrapper::subscribe(OnEvent* newSubscriber)
	{
		m_EventHandler->subscribe(newSubscriber);
	}

} // CW
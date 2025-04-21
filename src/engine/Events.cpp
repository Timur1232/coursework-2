#include "pch.h"
#include "Events.h"

#include "debug_utils/Log.h"
#include "debug_utils/Profiler.h"
#include "ProgramCore.h"

namespace CW {

	template<>
	class CoreDispatcher<void, void>
	{
	};


	template<>
	void CoreDispatcher<KeyPressedObs, sf::Event::KeyPressed>::operator()()
	{
		m_Target->OnKeyPressed(m_Event);
	}


	template<>
	void CoreDispatcher<KeyReleasedObs, sf::Event::KeyReleased>::operator()()
	{
		m_Target->OnKeyReleased(m_Event);
	}


	template<>
	void CoreDispatcher<MouseButtonPressedObs, sf::Event::MouseButtonPressed>::operator()()
	{
		m_Target->OnMouseButtonPressed(m_Event);
	}


	template<>
	void CoreDispatcher<MouseButtonReleasedObs, sf::Event::MouseButtonReleased>::operator()()
	{
		m_Target->OnMouseButtonReleased(m_Event);
	}


	template<>
	void CoreDispatcher<MouseMovedObs, sf::Event::MouseMoved>::operator()()
	{
		m_Target->OnMouseMoved(m_Event);
	}


	template<>
	void CoreDispatcher<MouseWheelScrolledObs, sf::Event::MouseWheelScrolled>::operator()()
	{
		m_Target->OnMouseWheelScrolled(m_Event);
	}


	template<>
	void CoreDispatcher<ClosedObs, sf::Event::Closed>::operator()()
	{
		m_Target->OnClosed();
	}


	template<>
	void CoreDispatcher<ResizedObs, sf::Event::Resized>::operator()()
	{
		m_Target->OnResized(m_Event);
	}


	template<class _EventSub = void, class _EventType = void, class... _EventSubArgs>
	void dispatchCoreEvent(OnEvent* target, const sf::Event& event)
	{
		if (auto d = CoreDispatcher<_EventSub, _EventType>(target, event); d.IsValid())
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

	EventHandler& EventHandler::Get()
	{
		static EventHandler handler;
		return handler;
	}

	void EventHandler::Reserve(size_t targetReserve, size_t eventReserve)
	{
		m_EventTargets.reserve(targetReserve);
		m_UserEvents.reserve(eventReserve);
	}

	size_t EventHandler::Subscribe(OnEvent* newSubscriber)
	{
		if (m_UnsubsCount)
		{
			m_EventTargets[m_EventTargets.size() - m_UnsubsCount] = newSubscriber;
			--m_UnsubsCount;
		}
		else
		{
			m_EventTargets.push_back(newSubscriber);
		}
		return m_EventTargets.size() - m_UnsubsCount - 1;
	}

	void EventHandler::Unsubscribe(size_t index)
	{
		++m_UnsubsCount;
		m_EventTargets[index] = m_EventTargets[m_EventTargets.size() - m_UnsubsCount];
		m_EventTargets[m_EventTargets.size() - m_UnsubsCount] = nullptr;
		m_EventTargets[index]->SetIndex(index);
	}

	void EventHandler::HandleEvents(sf::RenderWindow& window)
	{
		CW_PROFILE_FUNCTION();
		while (const std::optional event = window.pollEvent())
		{
			CW_PROFILE_SCOPE("outer loop");
			ImGui::SFML::ProcessEvent(window, *event);

			for (auto target : m_EventTargets)
			{
				CW_PROFILE_SCOPE("inner loop");
				if (target->IsAcceptingEvents())
				{
					dispatchCoreEvent<
						KeyPressedObs, sf::Event::KeyPressed,
						KeyReleasedObs, sf::Event::KeyReleased,
						MouseButtonPressedObs, sf::Event::MouseButtonPressed,
						MouseButtonReleasedObs, sf::Event::MouseButtonReleased,
						MouseMovedObs, sf::Event::MouseMoved,
						MouseWheelScrolledObs, sf::Event::MouseWheelScrolled,
						ClosedObs, sf::Event::Closed,
						ResizedObs, sf::Event::Resized
					>(target, *event);
				}
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
		if (auto d = UserDispatcher<_EventSub, _EventType>(target, event); d.IsValid())
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


	void EventHandler::HandleUserEvents()
	{
		CW_PROFILE_FUNCTION();
		for (auto& event : m_UserEvents)
		{
			CW_PROFILE_SCOPE("outer loop");
			for (auto target : m_EventTargets)
			{
				CW_PROFILE_SCOPE("inner loop");
				dispatchUserEvent<CW_USER_EVENTS_PAIRS>(target, event);
			}
		}
		m_UserEvents.clear();
	}

#endif

} // CW
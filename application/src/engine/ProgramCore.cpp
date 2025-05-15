#include "pch.h"
#include "ProgramCore.h"

#include <debug_utils/Log.h>
#include <debug_utils/Profiler.h>
#include "Events/CoreEvents.h"
#include "Events/UserEventHandler.h"
#include "Events/UserEvents.h"

namespace CW {

	ProgramCore::ProgramCore()
	{
		UserEventHandler::Get().Reserve(1024);
		CW_INFO("Core initialized.");
	}

	ProgramCore::~ProgramCore()
	{
		if (!m_Window->isOpen())
			m_Window->close();
		ImGui::SFML::Shutdown();
		CW_INFO("Closing program.");
	}

	void ProgramCore::Run()
	{
		CW_INFO("Starting main loop.");
		CW_START_PROFILE_SESSION();
		CW_PROFILE_FUNCTION();
		sf::Time deltaTime;

		while (m_App->IsRunning())
		{
			CW_PROFILE_SCOPE("main loop");

			deltaTime = m_DeltaClock.restart();

			pollEvents();
			pollUserEvents();

			ImGui::SFML::Update(*m_Window, deltaTime);

			if (m_App->IsPaused())
				m_App->PauseUpdate(deltaTime.asSeconds());
			else
				m_App->Update(deltaTime.asSeconds());

			m_Window->clear(m_App->GetClearColor());
			m_App->Draw(*m_Window);
			ImGui::SFML::Render(*m_Window);
			m_Window->display();
		}
		CW_END_PROFILE_SESSION();
	}


	template<class _EventType = void, class _SFMLEventType = void, class... _EventSubArgs>
	void dispatch_core_event(sf::Event& event, Application& app)
	{
		if (auto e = event.getIf<_SFMLEventType>())
		{
			_EventType dispached(*e);
			app.OnEvent(dispached);
		}
		dispatch_core_event<_EventSubArgs...>(event, app);
	}

	template<>
	void dispatch_core_event<void, void>(sf::Event& event, Application& app)
	{
	}

	void ProgramCore::pollEvents()
	{
		CW_PROFILE_FUNCTION();
		int eventsCount = 0;
		while (std::optional event = m_Window->pollEvent())
		{
			{
				CW_PROFILE_SCOPE("ImGui events");
				ImGui::SFML::ProcessEvent(*m_Window, *event);
			}

			if (event->is<sf::Event::Closed>())
			{
				WindowClosed closed;
				m_App->OnEvent(closed);
				onClosed();
			}
			else
			{
				CW_PROFILE_SCOPE("Dispacher function");
				dispatch_core_event<
					WindowResized, sf::Event::Resized,
					KeyPressed, sf::Event::KeyPressed,
					KeyReleased, sf::Event::KeyReleased,
					MouseWheelScrolled, sf::Event::MouseWheelScrolled,
					MouseButtonPressed, sf::Event::MouseButtonPressed,
					MouseButtonReleased, sf::Event::MouseButtonReleased,
					MouseMoved, sf::Event::MouseMoved
				>(*event, *m_App);
			}
			eventsCount++;
		}
	}


	template<class _EventType = void, class... _EventSubArgs>
	void dispatch_user_event(MyEvent& event, Application& app)
	{
		if (auto d = UserEventDispatcher<_EventType>(event))
		{
			d(app);
			return;
		}
		dispatch_user_event<_EventSubArgs...>(event, app);
	}

	template<>
	void dispatch_user_event<void>(MyEvent& event, Application& app)
	{
	}

	void ProgramCore::pollUserEvents()
	{
		CW_PROFILE_FUNCTION();
		for (auto& event : UserEventHandler::Get().GetEvents())
		{
			dispatch_user_event<CW_USER_EVENTS_LIST>(event, *m_App);
		}
		UserEventHandler::Get().ClearEvents();
	}

	void ProgramCore::SetApplication(std::unique_ptr<Application>&& app)
	{
		m_App = std::forward<std::unique_ptr<Application>>(app);

		m_Window = CreateUnique<sf::RenderWindow>(sf::VideoMode(m_App->GetWindowSize()), m_App->GetTitle());
		/*m_Window.setVerticalSyncEnabled(true);

		m_Window.setFramerateLimit(60);*/
		if (!ImGui::SFML::Init(*m_Window))
		{
			CW_CRITICAL("Failing initializing ImGui::SFML.");
		}
	}

	void ProgramCore::onKeyPressed(KeyPressed& e)
	{
		if (e.Data.code == sf::Keyboard::Key::F11)
		{
			m_Window->create(sf::VideoMode(m_App->GetWindowSize()), m_App->GetTitle(), reverseState());
		}
	}

	void ProgramCore::onClosed()
	{
		m_App->Close();
	}

	sf::State ProgramCore::reverseState()
	{
		switch (m_WindowState)
		{
		case sf::State::Windowed: m_WindowState = sf::State::Fullscreen; break;
		case sf::State::Fullscreen: m_WindowState = sf::State::Windowed; break;
		default: break;
		}
		return m_WindowState;
	}

} // CW
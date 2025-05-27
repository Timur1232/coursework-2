#include "pch.h"
#include "ProgramCore.h"

#include <debug_utils/Log.h>
#include <debug_utils/Profiler.h>
#include "Events/CoreEvents.h"
#include "Events/UserEventHandler.h"
#include "Events/UserEvents.h"
#include "Renderer.h"

namespace CW {

	ProgramCore::ProgramCore()
	{
		EventHandler::Get().Reserve(1024);
		CW_INFO("Core initialized.");
	}

	ProgramCore::~ProgramCore()
	{
		Renderer::Get().ReleaseRenderTarget();
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

			if (!m_App->IsPaused())
			{
				m_App->Update(deltaTime.asSeconds());
				m_App->UpdateLayers(deltaTime.asSeconds());
			}
			else
			{
				m_App->PausedUpdate(deltaTime.asSeconds());
				m_App->PausedUpdateLayers(deltaTime.asSeconds());
			}


			m_Window->clear(m_App->GetClearColor());
			m_App->Draw();
			m_App->DrawLayers();
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
		while (std::optional event = m_Window->pollEvent())
		{
			{
				CW_PROFILE_SCOPE("ImGui events");
				ImGui::SFML::ProcessEvent(*m_Window, *event);
			}

			// особые случаи ивентов
			if (event->is<sf::Event::Closed>())
			{
				WindowClosed closed;
				m_App->OnEvent(closed);
				onClosed();
			}
			else if (auto e = event->getIf<sf::Event::Resized>())
			{
				WindowResized resized(*e);
				onWindowResized(resized);
				m_App->OnEvent(resized);
			}
			// ивенты MouseButtonPressed, MouseButtonReleased и MouseMoved отправляются, только если не были обработаны ImGui
			else if (auto e = event->getIf<sf::Event::MouseButtonPressed>(); e && !ImGui::GetIO().WantCaptureMouse)
			{
				MouseButtonPressed pressed(*e);
				m_App->OnEvent(pressed);
			}
			else if (auto e = event->getIf<sf::Event::MouseButtonReleased>(); e && !ImGui::GetIO().WantCaptureMouse)
			{
				MouseButtonReleased released(*e);
				m_App->OnEvent(released);
			}
			else if (auto e = event->getIf<sf::Event::MouseMoved>(); e && !ImGui::GetIO().WantCaptureMouse)
			{
				MouseMoved moved(*e);
				m_App->OnEvent(moved);
			}
			else
			{
				CW_PROFILE_SCOPE("Dispacher function");
				dispatch_core_event<
					KeyPressed, sf::Event::KeyPressed,
					KeyReleased, sf::Event::KeyReleased,
					MouseWheelScrolled, sf::Event::MouseWheelScrolled
				>(*event, *m_App);
			}
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
		auto& events = EventHandler::Get().GetEvents();
		for (size_t i = 0; i < events.size(); ++i)
		{
			auto& event = events[i];
			dispatch_user_event<CW_USER_EVENTS_LIST>(event, *m_App);
		}
		EventHandler::Get().ClearEvents();
	}

	void ProgramCore::SetApplication(std::unique_ptr<Application>&& app)
	{
		m_App = std::forward<std::unique_ptr<Application>>(app);

		m_Window = CreateShared<sf::RenderWindow>(sf::VideoMode(m_App->GetWindowSize()), m_App->GetTitle());
		Renderer::Get().SetRenderTarget(m_Window);

		m_Window->setVerticalSyncEnabled(true);
		m_Window->setFramerateLimit(60);

		if (!ImGui::SFML::Init(*m_Window))
		{
			CW_CRITICAL("Failing initializing ImGui::SFML.");
		}

		m_App->Init();
	}

	void ProgramCore::onKeyPressed(KeyPressed& e)
	{
		if (e.Data.code == sf::Keyboard::Key::F11)
		{
			m_Window->create(sf::VideoMode(m_App->GetWindowSize()), m_App->GetTitle(), reverseState());
		}
	}

	void ProgramCore::onWindowResized(WindowResized& e)
	{
		Renderer::Get().SetDefaultViewSize(static_cast<sf::Vector2f>(e.Size));
		Renderer::Get().SetDefaultViewCenter({ static_cast<float>(e.Size.x) / 2.0f, static_cast<float>(e.Size.y) / 2.0f });
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
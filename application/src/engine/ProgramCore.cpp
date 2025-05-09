#include "pch.h"
#include "ProgramCore.h"

#include <debug_utils/Log.h>
#include <debug_utils/Profiler.h>
#include "UPSLimiter.h"

namespace CW {

	ProgramCore::ProgramCore()
	{
		CW_INFO("Core initialized.");
	}

	ProgramCore::~ProgramCore()
	{
		m_Renderer->CloseWindow();
		//ImGui::SFML::Shutdown();
		CW_INFO("Closing program.");
	}

	void ProgramCore::Run()
	{
		CW_INFO("Starting main loop.");
		CW_START_PROFILE_SESSION();
		CW_PROFILE_FUNCTION();

		std::thread renderThread([this] {
			while (m_RenderRunning)
			{
				m_Renderer->Render(m_DeltaTime);
				m_Renderer->CopyState(*m_App);
			}
		});

		while (m_App->IsRunning())
		{
			CW_PROFILE_SCOPE("main loop");

			m_DeltaTime = m_DeltaClock.restart();
			{
				std::lock_guard<std::mutex> lock(WINDOW_MUTEX);
				//auto& window = m_Renderer->GetWindow();
				EventHandler::Get().HandleEvents(WINDOW);
			}
#ifdef CW_USER_EVENTS_LIST
			EventHandler::Get().HandleUserEvents();
#endif
			{
				//std::lock_guard<std::mutex> lock(WINDOW_MUTEX);
				//auto& window = m_Renderer->GetWindow();
				//ImGui::SFML::Update(WINDOW, deltaTime);
			}

			//m_App->UpdateInterface();

			if (m_App->IsPaused())
				m_App->PauseUpdate(m_DeltaTime);
			else
				m_App->Update(m_DeltaTime);

			/*m_Renderer->GetWindow().load()->clear();
			m_App->Draw(*m_Renderer->GetWindow().load());
			ImGui::SFML::Render(*m_Renderer->GetWindow().load());
			m_Renderer->GetWindow().load()->display();*/

			m_UPSLimiter.Wait();
		}
		m_RenderRunning = false;

		renderThread.join();
		CW_END_PROFILE_SESSION();
	}

	void ProgramCore::SetApplication(std::unique_ptr<Application>&& app)
	{
		m_App = std::forward<std::unique_ptr<Application>>(app);
		m_App->SubscribeOnEvents();
	}

	void ProgramCore::SetRenderer(std::unique_ptr<Renderer>&& renderer)
	{
		m_Renderer = std::forward<std::unique_ptr<Renderer>>(renderer);
	}

	void ProgramCore::OnKeyPressed(const sf::Event::KeyPressed* e)
	{
		if (e->code == sf::Keyboard::Key::F11)
		{
			m_Renderer->RecreateWindow(reverseState());
		}
	}

	void ProgramCore::OnClosed()
	{
		m_App->Close();
	}

	void ProgramCore::OnUPSChange(const UPSChange* e)
	{
		m_UPSLimiter.SetUPS(e->UPS);
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
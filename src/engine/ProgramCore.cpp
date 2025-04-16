#include "ProgramCore.h"

#include <imgui-SFML.h>
#include <debug_utils/Log.h>
#include <debug_utils/Profiler.h>

namespace CW {

	ProgramCore::ProgramCore()
	{
		CW_INFO("Core initialized.");
	}

	ProgramCore::~ProgramCore()
	{
		if (!m_Window.isOpen())
			m_Window.close();
		ImGui::SFML::Shutdown();
		CW_INFO("Closing program.");
	}

	void ProgramCore::run()
	{
		CW_INFO("Starting main loop.");
		CW_START_PROFILE_SESSION();
		CW_PROFILE_FUNCTION();

		while (m_App->isRunning())
		{
			CW_PROFILE_SCOPE("main loop");
			m_DeltaTime = m_DeltaClock.restart();
			EventHandler::get().handleEvents(m_Window);
#ifdef CW_USER_EVENTS_LIST
			EventHandler::get().handleUserEvents();
#endif

			ImGui::SFML::Update(m_Window, m_DeltaTime);

			m_App->update(m_DeltaTime);

			m_Window.clear();
			m_App->draw(m_Window);
			ImGui::SFML::Render(m_Window);
			m_Window.display();
		}
		CW_END_PROFILE_SESSION();
	}

	void ProgramCore::setApplication(std::unique_ptr<Application>&& app)
	{
		m_App = std::forward<std::unique_ptr<Application>>(app);

		m_Window.create(sf::VideoMode(m_App->getWindowSize()), m_App->getTitle());
		m_Window.setVerticalSyncEnabled(true);

		m_Window.setFramerateLimit(60);
		if (!ImGui::SFML::Init(m_Window))
		{
			CW_CRITICAL("Failing initializing ImGui::SFML.");
		}

		m_App->subscribeOnEvents();
	}

	void ProgramCore::onKeyPressed(const sf::Event::KeyPressed* e)
	{
		if (e->code == sf::Keyboard::Key::F11)
		{
			m_Window.create(sf::VideoMode(m_App->getWindowSize()), m_App->getTitle(), reverseState());
		}
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
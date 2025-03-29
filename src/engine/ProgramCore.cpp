#include "ProgramCore.h"

#include <imgui-SFML.h>
#include <debug_utils/Log.h>

namespace CW_E {

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

		while (m_App->isRunning())
		{
			m_DeltaTime = m_DeltaClock.restart();
			m_EventHandler.handleEvents(m_Window);
#ifdef CW_USER_EVENTS_LIST
			m_EventHandler.handleUserEvents();
#endif

			ImGui::SFML::Update(m_Window, m_DeltaTime);

			m_App->update(m_DeltaTime);

			m_UpdateHandler.handleUpdates(m_DeltaTime);

			m_Window.clear();
			m_App->draw(m_Window);

			// TODO: render objects

			ImGui::SFML::Render(m_Window);
			m_Window.display();
		}
	}

	void ProgramCore::setApplication(std::unique_ptr<Application>&& app)
	{
		m_App = std::forward<std::unique_ptr<Application>>(app);

		m_Window.create(sf::VideoMode(m_App->getWindowSize()), m_App->getTitle());

		m_Window.setFramerateLimit(60);
		if (!ImGui::SFML::Init(m_Window))
		{
			CW_CRITICAL("Failing initializing ImGui::SFML.");
		}
	}

	EventHandler* ProgramCore::getEventHandler()
	{
		return &m_EventHandler;
	}

	UpdateHandler* ProgramCore::getUpdateHandler()
	{
		return &m_UpdateHandler;
	}

} // CW_E
#include "ProgramCore.h"

#include <imgui-SFML.h>
#include <debug/Log.h>

namespace CW {

	ProgramCore::ProgramCore(std::unique_ptr<Application>&& app)
		: m_App(std::forward<std::unique_ptr<Application>>(app)),
		  m_Window(sf::VideoMode(m_App->getWindowSize()), m_App->getTitle()),
		  m_EventHandler(1024)
	{
		m_Window.setFramerateLimit(60);
		if (!ImGui::SFML::Init(m_Window))
		{
			CW_CRITICAL("Failing initializing ImGui::SFML.");
		}
		m_App->eventSubscribtion(m_EventHandler);
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
			m_EventHandler.handleEvents(m_Window);
			ImGui::SFML::Update(m_Window, m_DeltaClock.restart());

			m_App->update();

			m_Window.clear();
			m_App->draw(m_Window);

			ImGui::SFML::Render(m_Window);
			m_Window.display();
		}
	}

} // CW
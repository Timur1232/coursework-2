#pragma once

#include <SFML/Graphics.hpp>

#include "Application.h"
#include "Events.h"

namespace CW_E {

	class ProgramCore
	{
	public:
		ProgramCore();
		ProgramCore(const ProgramCore&) = delete;
		ProgramCore(ProgramCore&&) = delete;

		~ProgramCore();

		// Основной цикл программы
		void run();

		void setApplication(std::unique_ptr<Application>&& app);

		EventHandler* getEventHandler();
		UpdateHandler* getUpdateHandler();

	private:
		std::unique_ptr<Application> m_App;

		sf::RenderWindow m_Window;
		sf::Clock m_DeltaClock;
		sf::Time m_DeltaTime;

		EventHandler m_EventHandler{ 1024, 1024 };
		UpdateHandler m_UpdateHandler{ 1024 };
	};

} // CW_E
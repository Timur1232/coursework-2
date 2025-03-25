#pragma once

#include <SFML/Graphics.hpp>

#include "Application.h"
#include "Events.h"

namespace CW {

	class ProgramCore
	{
	public:
		ProgramCore() = delete;
		ProgramCore(const ProgramCore&) = delete;
		ProgramCore(ProgramCore&&) = delete;

		ProgramCore(std::unique_ptr<Application>&& app);
		~ProgramCore();

		// Основной цикл программы
		void run();

	private:
		std::unique_ptr<Application> m_App;

		sf::RenderWindow m_Window;
		sf::Clock m_DeltaClock;

		EventHandler m_EventHandler;
	};

} // CW
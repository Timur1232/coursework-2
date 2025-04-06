#pragma once

#include <SFML/Graphics.hpp>

#include "Application.h"
#include "Events.h"

namespace CW_E {

	class ProgramCore
		: public OnKeyPressed
	{
	public:
		ProgramCore();
		ProgramCore(const ProgramCore&) = delete;
		ProgramCore(ProgramCore&&) = delete;

		~ProgramCore();

		// Основной цикл программы
		void run();

		void setApplication(std::unique_ptr<Application>&& app);

		void onKeyPressed(const sf::Event::KeyPressed* e) override;

		sf::State reverseState();

	private:
		std::unique_ptr<Application> m_App;

		sf::RenderWindow m_Window;
		sf::State m_WindowState = sf::State::Windowed;
		sf::Clock m_DeltaClock;
		sf::Time m_DeltaTime;
	};

} // CW_E
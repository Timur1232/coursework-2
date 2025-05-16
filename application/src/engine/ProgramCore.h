#pragma once

#include "pch.h"

#include "Application.h"
#include "Events/Event.h"
#include "Events/CoreEvents.h"

namespace CW {

	class ProgramCore
	{
	public:
		ProgramCore();
		ProgramCore(const ProgramCore&) = delete;
		ProgramCore(ProgramCore&&) = delete;

		~ProgramCore();

		// Основной цикл программы
		void Run();

		void SetApplication(std::unique_ptr<Application>&& app);

	private:
		sf::State reverseState();

		void pollEvents();
		void pollUserEvents();

		void onClosed();
		void onKeyPressed(KeyPressed& e);

	private:
		Unique<Application> m_App;

		Shared<sf::RenderWindow> m_Window;
		sf::State m_WindowState = sf::State::Windowed;
		sf::Clock m_DeltaClock;
	};

} // CW
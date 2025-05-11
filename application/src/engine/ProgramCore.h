#pragma once

#include "pch.h"

#include "Application.h"
#include "Events.h"
#include "CoreEvents.h"

#include "Types.h"

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
		//void onUPSChange();

	private:
		Unique<Application> m_App;

		Unique<sf::RenderWindow> m_Window;
		sf::State m_WindowState = sf::State::Windowed;
		sf::Clock m_DeltaClock;
		sf::Time m_DeltaTime;

		//UPSLimiter m_UPS{ 60 };
	};

} // CW
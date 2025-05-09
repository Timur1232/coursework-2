#pragma once

#include "pch.h"

#include "Application.h"
#include "Events.h"
#include "UPSLimiter.h"

namespace CW {

	class ProgramCore
		: public KeyPressedObs
	{
	public:
		ProgramCore();
		ProgramCore(const ProgramCore&) = delete;
		ProgramCore(ProgramCore&&) = delete;

		~ProgramCore();

		// Основной цикл программы
		void Run();

		void SetApplication(std::unique_ptr<Application>&& app);

		void OnKeyPressed(const sf::Event::KeyPressed* e) override;

	private:
		sf::State reverseState();

	private:
		std::unique_ptr<Application> m_App;

		sf::RenderWindow m_Window;
		sf::State m_WindowState = sf::State::Windowed;
		sf::Clock m_DeltaClock;
		sf::Time m_DeltaTime;

		UPSLimiter m_UPS{ 60 };
	};

} // CW
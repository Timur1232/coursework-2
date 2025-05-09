#pragma once

#include "pch.h"

#include "Application.h"
#include "Renderer.h"
#include "Events.h"
#include "UPSLimiter.h"

#include "CW_EventConfig.h"

namespace CW {

	class ProgramCore
		: public KeyPressedObs,
		  public ClosedObs,
		  public UPSChangeObs
	{
	public:
		ProgramCore();
		ProgramCore(const ProgramCore&) = delete;
		ProgramCore(ProgramCore&&) = delete;

		~ProgramCore();

		// Основной цикл программы
		void Run();

		void SetApplication(std::unique_ptr<Application>&& app);
		void SetRenderer(std::unique_ptr<Renderer>&& renderer);

		void OnKeyPressed(const sf::Event::KeyPressed* e) override;
		void OnClosed() override;
		void OnUPSChange(const UPSChange* e) override;

	private:
		sf::State reverseState();

	private:
		std::unique_ptr<Application> m_App;
		std::unique_ptr<Renderer> m_Renderer;
		UPSLimiter m_UPSLimiter{ 60 };

		std::atomic<bool> m_RenderRunning = true;

		sf::State m_WindowState = sf::State::Windowed;
		sf::Clock m_DeltaClock;
		sf::Time m_DeltaTime;

	};

} // CW
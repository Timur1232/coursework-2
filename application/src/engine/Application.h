#pragma once

#include "pch.h"

#include "IDrawable.h"
#include "IUpdate.h"
#include "Events/Event.h"
#include "Events/EventInterface.h"

namespace CW {

	using namespace std::chrono_literals;

	class Application
		: public IDrawable,
		  public IUpdate,
		  public IOnEvent
	{
	public:
		Application() = delete;
		Application(int width, int height, const char* title);
		virtual ~Application() = default;

		virtual void UpdateInterface() = 0;
		virtual void PauseUpdate(sf::Time deltaTime) = 0;

		sf::Vector2u GetWindowSize() const;
		const char* GetTitle() const;
		sf::Color GetClearColor() const { return m_ClearColor; }

		bool IsRunning() const;
		void Close();

		bool IsPaused() const;
		void SwitchPause();

	protected:
		sf::Vector2u m_WindowSize;
		const char* m_WindowTitle;
		sf::Color m_ClearColor = sf::Color::Black;

		bool m_Running = true;
		bool m_Pause = false;
	};

} // CW
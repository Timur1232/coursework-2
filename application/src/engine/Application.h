#pragma once

#include "pch.h"

#include "Events.h"
#include "IDrawable.h"
#include "IUpdate.h"
#include "ApplicationState.h"

namespace CW {

	using namespace std::chrono_literals;

	class Application
		: public IDrawable,
		  public IUpdate,
		  public virtual OnEvent
	{
	public:
		Application() = delete;
		Application(int width, int height, const char* title);
		virtual ~Application() = default;

		virtual void UpdateInterface() = 0;
		virtual void PauseUpdate(sf::Time deltaTime) = 0;

		sf::Vector2u GetWindowSize() const;
		const char* GetTitle() const;

		bool IsRunning() const;
		void Close();

		bool IsPaused() const;
		void SwitchPause();

		size_t GetUPSLimit() const { return m_UPSLimit; }
		//virtual std::unique_ptr<ApplicationState> CollectState() const = 0;

	protected:
		sf::Vector2u m_WindowSize;
		const char* m_WindowTitle;

		bool m_Running = true;
		bool m_Pause = false;

		size_t m_UPSLimit = 60;
	};

} // CW
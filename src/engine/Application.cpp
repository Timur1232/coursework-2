#include "pch.h"
#include "Application.h"

#include "debug_utils/Log.h"

namespace CW {

    Application::Application(int width, int height, const char* title)
        : m_WindowSize(width, height), m_WindowTitle(title)
    {
    }

	sf::Vector2u Application::GetWindowSize() const
	{
		return m_WindowSize;
	}

	const char* Application::GetTitle() const
	{
		return m_WindowTitle;
	}

	bool Application::IsRunning() const
	{
		return m_Running;
	}

	void Application::Close()
	{
		m_Running = false;
	}

	bool Application::IsPaused() const
	{
		return m_Pause;
	}

	void Application::SwitchPause()
	{
		m_Pause = !m_Pause;
	}

} // CW
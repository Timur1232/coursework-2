#include "Application.h"

#include "debug_utils/Log.h"

namespace CW {

    Application::Application(int width, int height, const char* title)
        : m_WindowSize(width, height), m_WindowTitle(title)
    {
    }

	sf::Vector2u Application::getWindowSize() const
	{
		return m_WindowSize;
	}

	const char* Application::getTitle() const
	{
		return m_WindowTitle;
	}

	bool Application::isRunning() const
	{
		return m_Running;
	}

	void Application::close()
	{
		m_Running = false;
	}

} // CW
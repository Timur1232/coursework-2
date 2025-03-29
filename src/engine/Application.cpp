#include "Application.h"

#include "debug_utils/Log.h"

namespace CW_E {

    Application::Application(int width, int height, const char* title,
		EventHandlerWrapper eventHandler,
		UpdateHandlerWrapper updateHandler)
        : m_WindowSize(width, height), m_WindowTitle(title), m_EventHandler(eventHandler), m_UpdateHandler(updateHandler)
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

	EventHandlerWrapper Application::getEventHandler() const
	{
		return m_EventHandler;
	}

	UpdateHandlerWrapper Application::getUpdateHandler() const
	{
		return m_UpdateHandler;
	}

} // CW_E
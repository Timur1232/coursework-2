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

	void Application::setEventHandler(EventHandlerWrapper handler)
	{
		if (!m_EventHandler)
			m_EventHandler = handler;
		else
			CW_ASSERT("Not allowed to set EventHandler more than one time!");
	}

	EventHandlerWrapper Application::getEventHandler() const
	{
		if (!m_EventHandler)
			CW_ASSERT("Using of getEventHandler() before core initialization! Use it coreInit() instead.");
		return m_EventHandler;
	}

	RenderWrapper::RenderWrapper(sf::RenderWindow& window)
		: m_Window(window)
	{
	}

	void RenderWrapper::draw(const sf::Drawable& drawable)
	{
		m_Window.draw(drawable);
	}

} // CW
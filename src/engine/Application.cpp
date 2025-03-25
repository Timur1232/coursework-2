#include "Application.h"

namespace CW {

    Application::Application(int width, int height, const char* title)
        : m_WindowSize(width, height), m_WindowTitle(title)
    {
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
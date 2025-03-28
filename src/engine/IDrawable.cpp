#include "IDrawable.h"

namespace CW {

	RenderWrapper::RenderWrapper(sf::RenderWindow& window)
		: m_Window(window)
	{
	}

	void RenderWrapper::draw(const sf::Drawable& drawable)
	{
		m_Window.draw(drawable);
	}

	void RenderWrapper::setView(const sf::View& view)
	{
		m_Window.setView(view);
	}

} // CW
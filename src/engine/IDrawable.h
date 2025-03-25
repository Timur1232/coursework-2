#pragma once

#include <SFML/Graphics.hpp>

namespace CW {

	class RenderWrapper
	{
	public:
		RenderWrapper(sf::RenderWindow& window);

		void draw(const sf::Drawable& drawable);

	private:
		sf::RenderWindow& m_Window;
	};

	class IDrawable
	{
	public:
		virtual void draw(RenderWrapper) = 0;
	};

} // CW
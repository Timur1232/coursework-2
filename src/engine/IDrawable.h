#pragma once

#include "pch.h"

namespace CW {

	/*class RenderWrapper
	{
	public:
		RenderWrapper(sf::RenderWindow& window);

		void draw(const sf::Drawable& drawable);
		void setView(const sf::View& view);

	private:
		sf::RenderWindow& m_Window;
	};*/

	class IDrawable
	{
	public:
		virtual ~IDrawable() = default;
		virtual void Draw(sf::RenderWindow&) const = 0;
	};

} // CW
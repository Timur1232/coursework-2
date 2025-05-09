#pragma once

#include "pch.h"

namespace CW {

	class IDrawable
	{
	public:
		virtual ~IDrawable() = default;
		virtual void Draw(sf::RenderWindow&) = 0;
	};


	template <class T>
	concept has_draw = requires(T d, sf::RenderWindow & render)
	{
		{ d.Draw(render) };
	};

} // CW
#pragma once

#include "Window.h"

namespace CW
{

	class IDrawable
	{
	public:
		virtual void draw(const Window&) const = 0;
	};

} // CW
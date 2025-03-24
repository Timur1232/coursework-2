#pragma once

#include "Window.h"
#include "camera/Camera2D.h"

namespace CW
{

	class IDrawable
	{
	public:
		virtual void draw(const Window&, const Camera2D&) const = 0;
	};

} // CW
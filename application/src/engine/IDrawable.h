#pragma once

#include "pch.h"

namespace CW {

	class IDrawable
	{
	public:
		virtual ~IDrawable() = default;
		virtual void Draw() = 0;
	};


	template <class T>
	concept has_draw = requires(T d)
	{
		{ d.Draw() };
	};

} // CW
#pragma once

#include "IDrawable.h"

namespace CW
{

	class Terrain
		: public IDrawable
	{
	public:
		void draw(const Window&) const override;
		// TODO
	};

} // CW

#pragma once

#include "pch.h"

namespace CW {

	class IUpdate
	{
	public:
		virtual ~IUpdate() = default;
		virtual void Update(sf::Time deltaTime) = 0;
	};


	template <class T>
	concept has_update = requires(T u, sf::Time deltaTime)
	{
		{ u.Update(deltaTime) };
	};

} // CW
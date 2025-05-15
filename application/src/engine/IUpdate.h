#pragma once

#include "pch.h"

namespace CW {

	class IUpdate
	{
	public:
		virtual ~IUpdate() = default;
		virtual void Update(float deltaTime) = 0;
	};


	template <class T>
	concept has_update = requires(T u, float deltaTime)
	{
		{ u.Update(deltaTime) };
	};

} // CW
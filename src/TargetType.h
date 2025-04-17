#pragma once

#include "pch.h"

namespace CW {

	enum class TargetType
	{
		None = 0,
		Navigation,
		Recource
	};

	TargetType opposite_target_type(TargetType type);

} // CW
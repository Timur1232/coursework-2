#include "pch.h"
#include "BitDirection.h"

namespace CW {

	byte angle_to_bit_direction(sf::Angle angle)
	{
		bool isDiagonal = in_between_abs(angle.asRadians(), angle::PI_4, angle::PI_6)
			|| in_between_abs(angle.asRadians(), angle::PI_2 + angle::PI_4, angle::PI_6);
		if (isDiagonal)
		{
			if (auto quarter = angle::quarter(angle); quarter == angle::Quarter::First)
				return DirectionBit::Right | DirectionBit::Up;
			else if (quarter == angle::Quarter::Second)
				return DirectionBit::Left | DirectionBit::Up;
			else if (quarter == angle::Quarter::Third)
				return DirectionBit::Left | DirectionBit::Down;
			else
				return DirectionBit::Right | DirectionBit::Down;
		}
		else
		{
			if (in_between(angle.asRadians(), 0.0f, angle::PI_6))
				return DirectionBit::Right;
			else if (in_between(angle.asRadians(), angle::PI_2, angle::PI_6))
				return DirectionBit::Up;
			else if (in_between(angle.asRadians(), -angle::PI_2, angle::PI_6))
				return DirectionBit::Down;
			else
				return DirectionBit::Left;
		}
	}

} // CW
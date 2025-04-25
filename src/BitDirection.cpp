#include "pch.h"
#include "BitDirection.h"

namespace CW {

	uint8_t angle_to_bit_direction(sf::Angle angle)
	{
		uint8_t retBits = 0;
		bool isDiagonal = std::abs(angle.asRadians()) < (angle::PI / 4.0f + angle::PI / 6.0f) 
			&& std::abs(angle.asRadians()) > (angle::PI / 4.0f - angle::PI / 6.0f);
		if (isDiagonal)
		{
			if (angle.asRadians() > 0)
				retBits |= Right;
			else
				retBits |= Left;

			if (angle.asRadians() > 0)
				retBits |= Up;
			else
				retBits |= Down;
		}
		else
		{
			if (std::abs(angle.asRadians()) > std::abs(angle.asRadians()))
			{
				if (angle.asRadians() > 0)
					retBits |= Right;
				else
					retBits |= Left;
			}
			else
			{
				if (angle.asRadians() > 0)
					retBits |= Up;
				else
					retBits |= Down;
			}
		}
		return retBits;
	}

} // CW
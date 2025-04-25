#pragma once

#include "pch.h"

#include "utils/utils.h"



namespace CW {

	enum BitDirection : uint8_t
	{
		None = 0,
		Right = bit(0),
		Left = bit(1),
		Up = bit(2),
		Down = bit(3)
	};

	const std::unordered_map<uint8_t, sf::Angle> DIRECTION_ANGLE_TABLE = {
		{Right, sf::Angle::Zero},
		{Right | Up, sf::degrees(45.0f)},
		{Up, sf::degrees(90.0f)},
		{Left | Up, sf::degrees(135.0f)},
		{Left, sf::degrees(180.0f)},
		{Right | Down, sf::degrees(-45.0f)},
		{Down, sf::degrees(-90.0f)},
		{Left | Down, sf::degrees(-135.0f)}
	};

	uint8_t angle_to_bit_direction(sf::Angle vec);

} // CW
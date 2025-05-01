#pragma once

#include "pch.h"

#include "utils/utils.h"



namespace CW {

	namespace DirectionBit
	{
		constexpr uint8_t None  = 0u;
		constexpr uint8_t Right = static_cast<uint8_t>(bit(0));
		constexpr uint8_t Left  = static_cast<uint8_t>(bit(1));
		constexpr uint8_t Up    = static_cast<uint8_t>(bit(2));
		constexpr uint8_t Down  = static_cast<uint8_t>(bit(3));
	};

	const std::unordered_map<uint8_t, sf::Angle> DIRECTION_ANGLE_TABLE = {
		{DirectionBit::Right, sf::Angle::Zero},
		{DirectionBit::Right | DirectionBit::Up, sf::degrees(45.0f)},
		{DirectionBit::Up, sf::degrees(90.0f)},
		{DirectionBit::Left | DirectionBit::Up, sf::degrees(135.0f)},
		{DirectionBit::Left, sf::degrees(-180.0f)},
		{DirectionBit::Right | DirectionBit::Down, sf::degrees(-45.0f)},
		{DirectionBit::Down, sf::degrees(-90.0f)},
		{DirectionBit::Left | DirectionBit::Down, sf::degrees(-135.0f)}
	};

	uint8_t angle_to_bit_direction(sf::Angle vec);

} // CW
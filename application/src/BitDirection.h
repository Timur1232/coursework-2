#pragma once
#include "pch.h"

#include "utils/utils.h"

namespace CW {

	namespace DirectionBit
	{
		constexpr byte None  = 0u;
		constexpr byte Right = static_cast<uint8_t>(bit(0));
		constexpr byte Left  = static_cast<uint8_t>(bit(1));
		constexpr byte Up    = static_cast<uint8_t>(bit(2));
		constexpr byte Down  = static_cast<uint8_t>(bit(3));
	};

	const std::unordered_map<byte, sf::Angle> DIRECTION_ANGLE_TABLE = {
		{DirectionBit::Right, sf::Angle::Zero},
		{DirectionBit::Right | DirectionBit::Up, sf::degrees(45.0f)},
		{DirectionBit::Up, sf::degrees(90.0f)},
		{DirectionBit::Left | DirectionBit::Up, sf::degrees(135.0f)},
		{DirectionBit::Left, sf::degrees(-180.0f)},
		{DirectionBit::Right | DirectionBit::Down, sf::degrees(-45.0f)},
		{DirectionBit::Down, sf::degrees(-90.0f)},
		{DirectionBit::Left | DirectionBit::Down, sf::degrees(-135.0f)}
	};

	byte angle_to_bit_direction(sf::Angle vec);

} // CW
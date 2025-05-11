#pragma once
#include "pch.h"

#include "Events.h"
#include "TargetType.h"

namespace CW {

	class CreateBeacon
		: public CW::Event
	{
	public:
		sf::Vector2f Position;
		CW::TargetType Type;
		uint8_t BitDirection;

		CreateBeacon(sf::Vector2f position, CW::TargetType type, uint8_t bitDirection)
			: Position(position), Type(type), BitDirection(bitDirection)
		{
		}
		EventType GetEventType() const { return EventType::CreateBeacon; }
		static EventType GetStaticEventType() { return EventType::CreateBeacon; }
	};

} // CW
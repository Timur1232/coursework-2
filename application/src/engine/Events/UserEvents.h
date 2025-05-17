#pragma once
#include "pch.h"

#include "Event.h"
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

	class CloseApp
		: public CW::Event
	{
	public:
		CloseApp() = default;
		EventType GetEventType() const { return EventType::CloseApp; }
		static EventType GetStaticEventType() { return EventType::CloseApp; }
	};

	class StartSimulation
		: public CW::Event
	{
	public:
		StartSimulation() = default;
		EventType GetEventType() const { return EventType::StartSimulation; }
		static EventType GetStaticEventType() { return EventType::StartSimulation; }
	};

} // CW
#pragma once

#include "pch.h"

#include "EventInterface.h"
#include "TargetType.h"

struct CreateBeacon
{
	sf::Vector2f Position;
	CW::TargetType Type;
	uint8_t BitDirection;

};

class CreateBeaconObs
	: virtual public CW::OnEvent
{
public:
	virtual ~CreateBeaconObs() = default;
	virtual void OnCreateBeacon(const CreateBeacon*) = 0;
};

// Для определения пользовательских типов ивентов нужно объявить в этом файле макрос CW_USER_EVENTS_LIST
// и записать через запятую типы отправляемых ивентов
#define CW_USER_EVENTS_LIST CreateBeacon

// Если определены пользовательские типы ивентов, то необходимо объявить макрос CW_USER_EVENTS_PAIRS,
// в котором через запятую указывается тип цели для отправления ивента и тип самого ивента
// <тип_цели>, <тип_ивента>, <тип_цели>, <тип_ивента>, ...
#define CW_USER_EVENTS_PAIRS CreateBeaconObs, CreateBeacon

// Пример
// 
//struct EventData
//{
//	int a;
//};
//
//class MyEventReciever
//	: virtual public CW::OnEvent
//{
//public:
//	virtual void onE(const EventData*) = 0;
//};
//
//
//struct OtherEventData
//{
//	float f;
//	int i;
//};
//
//class OtherReciever
//	:virtual public CW::OnEvent
//{
//public:
//	virtual void onOtherEvent(const OtherEventData*) = 0;
//};
//
//
//struct Thing {};
//
//class OnThing
//	: virtual public CW::OnEvent
//{
//public:
//	virtual void gimme(void) = 0;
//};

// #define CW_USER_EVENTS_LIST EventData, OtherEventData, Thing
// #define CW_USER_EVENTS_PAIRS MyEventReciever, EventData, OtherReciever, OtherEventData, OnThing, Thing
#pragma once

#include "EventInterface.h"

#include "SFML/Graphics.hpp"

#include "BeaconType.h"

struct CreateBeacon
{
	sf::Vector2f position;
	CW::BeaconType type;
};

class OnCreateBeacon
	: virtual public CW_E::OnEvent
{
public:
	virtual ~OnCreateBeacon() = default;
	virtual void onCreateBeacon(const CreateBeacon*) = 0;
};

// Для определения пользовательских типов ивентов нужно объявить в этом файле макрос CW_USER_EVENTS_LIST
// и записать через запятую типы отправляемых ивентов
#define CW_USER_EVENTS_LIST CreateBeacon

// Если определены пользовательские типы ивентов, то необходимо объявить макрос CW_USER_EVENTS_PAIRS,
// в котором через запятую указывается тип цели для отправления ивента и тип самого ивента
// <тип_цели>, <тип_ивента>, <тип_цели>, <тип_ивента>, ...
#define CW_USER_EVENTS_PAIRS OnCreateBeacon, CreateBeacon

// Пример
// 
//struct EventData
//{
//	int a;
//};
//
//class MyEventReciever
//	: virtual public CW_E::OnEvent
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
//	:virtual public CW_E::OnEvent
//{
//public:
//	virtual void onOtherEvent(const OtherEventData*) = 0;
//};
//
//
//struct Thing {};
//
//class OnThing
//	: virtual public CW_E::OnEvent
//{
//public:
//	virtual void gimme(void) = 0;
//};

// #define CW_USER_EVENTS_LIST EventData, OtherEventData, Thing
// #define CW_USER_EVENTS_PAIRS MyEventReciever, EventData, OtherReciever, OtherEventData, OnThing, Thing
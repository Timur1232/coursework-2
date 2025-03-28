#pragma once

#include "EventInterface.h"

//#define CW_DEFINE_USER_DISPATCHER(Reciever_t, Event_t, code, ...) template<> void UserDispatcher<MyEventReciever, EventData>::operator()() { code; } CW_DEFINE_USER_DISPATCHER(__VA_ARGS__)

struct EventData
{
	int a;
};

class MyEventReciever
	: virtual public CW::OnEvent
{
public:
	virtual void onE(const EventData*) = 0;
};

// ƒл€ определени€ пользовательских типов ивентов нужно объ€вить в этом файле макрос CW_USER_EVENTS_LIST
// и записать через зап€тую типы отправл€емых ивентов
#define CW_USER_EVENTS_LIST EventData

// ≈сли определены пользовательские типы ивентов, то необходимо объ€вить макрос CW_USER_EVENTS_PAIRS,
// в котором через зап€тую указываетс€ тип цели дл€ отправлени€ ивента и тип самого ивента
// <тип_цели>, <тип_ивента>, <тип_цели>, <тип_ивента>, ...
#define CW_USER_EVENTS_PAIRS MyEventReciever, EventData
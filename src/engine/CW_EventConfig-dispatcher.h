#pragma once

#include "CW_EventConfig.h"
#include "UserEvent.h"

template<>
void CW::UserDispatcher<MyEventReciever, EventData>::operator()()
{
    target->onE(event);
}

template<>
void CW::UserDispatcher<OtherReciever, OtherEventData>::operator()()
{
    target->onOtherEvent(event);
}

template<>
void CW::UserDispatcher<OnThing, Thing>::operator()()
{
    target->gimme();
}
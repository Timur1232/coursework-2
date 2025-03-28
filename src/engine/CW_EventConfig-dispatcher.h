#pragma once

#include "CW_EventConfig.h"
#include "UserEvent.h"

template<>
void CW::UserDispatcher<MyEventReciever, EventData>::operator()()
{
    target->onE(event);
}
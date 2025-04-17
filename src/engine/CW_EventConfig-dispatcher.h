#pragma once

#include "pch.h"

#include "CW_EventConfig.h"
#include "UserEvent.h"

template<>
void CW::UserDispatcher<OnCreateBeacon, CreateBeacon>::operator()()
{
    target->onCreateBeacon(event);
}
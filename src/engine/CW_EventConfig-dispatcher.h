#pragma once

#include "CW_EventConfig.h"
#include "UserEvent.h"

template<>
void CW_E::UserDispatcher<OnCreateBeacon, CreateBeacon>::operator()()
{
    target->onCreateBeacon(event);
}
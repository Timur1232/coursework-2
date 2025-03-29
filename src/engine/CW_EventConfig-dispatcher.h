#pragma once

#include "CW_EventConfig.h"
#include "UserEvent.h"

template<>
void CW_E::UserDispatcher<OnBeaconDischarge, BeaconDischarge>::operator()()
{
    target->onBeaconDischarge(event);
}
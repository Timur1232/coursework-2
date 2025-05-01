#pragma once

#include "pch.h"

#include "CW_EventConfig.h"
#include "UserEvent.h"

template<>
void CW::UserDispatcher<CreateBeaconObs, CreateBeacon>::operator()()
{
    m_Target->OnCreateBeacon(m_Event);
}
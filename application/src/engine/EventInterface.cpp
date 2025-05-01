#include "pch.h"
#include "EventInterface.h"

#include "Events.h"

namespace CW {

    bool OnEvent::IsAcceptingEvents() const
    {
        return true;
    }

    void OnEvent::SubscribeOnEvents()
    {
        m_EventRecieverIndex = EventHandler::Get().Subscribe(this);
    }

    void OnEvent::UnsubscribeOnEvents() const
    {
        EventHandler::Get().Unsubscribe(m_EventRecieverIndex);
    }

    size_t OnEvent::GetIndex() const
    {
        return m_EventRecieverIndex;
    }

    void OnEvent::SetIndex(size_t index)
    {
        m_EventRecieverIndex = index;
    }

} // CW
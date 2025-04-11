#include "EventInterface.h"

#include "Events.h"

namespace CW {

    bool OnEvent::isAcceptingEvents() const
    {
        return true;
    }

    void OnEvent::subscribeOnEvents()
    {
        m_EventRecieverIndex = EventHandler::get().subscribe(this);
    }

    void OnEvent::unsubscribeOnEvents() const
    {
        EventHandler::get().unsubscribe(m_EventRecieverIndex);
    }

    size_t OnEvent::getIndex() const
    {
        return m_EventRecieverIndex;
    }

    void OnEvent::setIndex(size_t index)
    {
        m_EventRecieverIndex = index;
    }

} // CW
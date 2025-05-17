#pragma once
#include <pch.h>

#include "Events/Event.h"
#include "IDrawable.h"
#include "IUpdate.h"

namespace CW {

    class Layer
        : public IDrawable,
          public IUpdate,
          public IOnEvent
    {
    public:
        Layer() = default;
        virtual ~Layer() = default;

        bool IsUpdateActive() const { return m_UpdateActive; }
        bool IsDrawActive() const { return m_DrawActive; }
        bool IsEventsActive() const { return m_EventsActive; }

        void SetUpdateActive(bool b) { m_UpdateActive = b; }
        void SetDrawActive(bool b) { m_DrawActive = b; }
        void SetEventsActive(bool b) { m_EventsActive = b; }

    protected:
        bool m_UpdateActive = true;
        bool m_DrawActive = true;
        bool m_EventsActive = true;
    };

} // CW
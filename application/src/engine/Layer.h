#pragma once
#include <pch.h>

#include "Events/Event.h"

namespace CW {

    class Layer
    {
    public:
        Layer() = default;
        virtual ~Layer() = default;

        virtual void Update(float deltaTime) = 0;
        virtual void OnEvent(Event& event) = 0;
        virtual void Draw(sf::RenderWindow& render) = 0;

        bool IsUpdateActive() const { return m_UpdateActive; }
        bool IsDrawActive() const { return m_DrawActive; }
        bool IsEventsActive() const { return m_EventsActive; }

        void SetUpdateActive(bool b) { m_UpdateActive = b; }
        void SetDrawActive(bool b) { m_DrawActive = b; }
        void SetEventsActive(bool b) { m_EventsActive = b; }

    private:
        bool m_UpdateActive = true;
        bool m_DrawActive = true;
        bool m_EventsActive = true;
    };

} // CW
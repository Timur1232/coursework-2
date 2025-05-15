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
        virtual bool OnEvent(Event& event) = 0;

        virtual bool IsActive() const { return true; }
    };

} // CW
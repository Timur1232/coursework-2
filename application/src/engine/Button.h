#pragma once
#include "pch.h"

#include "Object.h"
#include "Events/EventInterface.h"
#include "Events/CoreEvents.h"
#include "IDrawable.h"
#include "CollisionStrategy.h"

namespace CW {

    class Button
        : public IOnEvent
    {
    public:
        Button() = default;
        Button(Shared<ICollisionStrategy> collisionChecker);
        Button(Shared<ICollisionStrategy> collisionChecker,
            const std::function<bool(MouseButtonPressed&)>& onClickCallback,
            const std::function<bool(MouseMoved&)>& onHowerCallback);
        virtual ~Button() = default;

        void OnEvent(Event& event) override;

        void SetCollisionChecker(Shared<ICollisionStrategy> checker) { m_CollisionChecker = checker; }
        Shared<ICollisionStrategy> GetCollisionChecker() const { return m_CollisionChecker; }

        void SetOnClickCallback(const std::function<bool(MouseButtonPressed&)>& callback) { m_OnClickCallback = callback; }
        void SetOnHowerCallback(const std::function<bool(MouseMoved&)>& callback) { m_OnHowerCallback = callback; }

    private:
        Shared<ICollisionStrategy> m_CollisionChecker;
        std::function<bool(MouseButtonPressed&)> m_OnClickCallback{ [](MouseButtonPressed&) { return false; } };
        std::function<bool(MouseMoved&)> m_OnHowerCallback{ [](MouseMoved&) { return false; } };
    };

} // CW

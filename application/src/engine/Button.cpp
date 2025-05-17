#include "pch.h"
#include "Button.h"

#include "debug_utils/Log.h"

namespace CW {

    Button::Button(Shared<ICollisionStrategy> collisionChecker)
        : m_CollisionChecker(collisionChecker)
    {
    }

    Button::Button(Shared<ICollisionStrategy> collisionChecker,
        const std::function<bool(MouseButtonPressed&)>& onClickCallback,
        const std::function<bool(MouseMoved&)>& onHowerCallback)
        : m_CollisionChecker(collisionChecker),
          m_OnClickCallback(onClickCallback),
          m_OnHowerCallback(onHowerCallback)
    {
    }

    void Button::OnEvent(Event& event)
    {
        if (!m_CollisionChecker)
        {
            CW_ERROR("Need to provide collision detection strategy!");
            return;
        }

        EventDispatcher dispatcher(event);
        dispatcher.Dispach<MouseButtonPressed>(
            [this](MouseButtonPressed& e) {
                if (m_CollisionChecker->CheckCollision(static_cast<sf::Vector2f>(e.Data.position)))
                    return m_OnClickCallback(e);
                return false;
            });
        dispatcher.Dispach<MouseMoved>(
            [this](MouseMoved& e) {
                if (m_CollisionChecker->CheckCollision(static_cast<sf::Vector2f>(e.Data.position)))
                    return m_OnHowerCallback(e);
                return false;
            });
    }

} // CW
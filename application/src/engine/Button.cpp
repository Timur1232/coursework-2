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
        const std::function<bool(MouseMoved&)>& onEnterCallback,
        const std::function<bool(MouseMoved&)>& onExitCallback)
        : m_CollisionChecker(collisionChecker),
          m_OnClickCallback(onClickCallback),
          m_OnEnterCallback(onEnterCallback),
          m_OnExitCallback(onExitCallback)
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
        if (dispatcher.Dispach<MouseButtonPressed>(
            [this](MouseButtonPressed& e) {
                if (m_CollisionChecker->CheckCollision(static_cast<sf::Vector2f>(e.Data.position)))
                    return m_OnClickCallback(e);
                return false;
            }))
        {
            return;
        }
        dispatcher.Dispach<MouseMoved>(
            [this](MouseMoved& e) {
                sf::Vector2f mousePos = static_cast<sf::Vector2f>(e.Data.position);
                if (!m_Entered && m_CollisionChecker->CheckCollision(mousePos))
                {
                    m_Entered = true;
                    return m_OnEnterCallback(e);
                }
                if (m_Entered && !m_CollisionChecker->CheckCollision(mousePos))
                {
                    m_Entered = false;
                    return m_OnExitCallback(e);
                }
                return false;
            });
    }

} // CW
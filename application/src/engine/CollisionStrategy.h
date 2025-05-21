#pragma once
#include "pch.h"

namespace CW {

    class ICollisionStrategy
    {
    public:
        ICollisionStrategy() = default;
        virtual ~ICollisionStrategy() = default;
        virtual bool CheckCollision(sf::Vector2f point) const = 0;
    };


    class RectCollision
        : public ICollisionStrategy
    {
    public:
        RectCollision() = default;
        RectCollision(sf::Vector2f position, sf::Vector2f size)
            : m_Rect(position, size)
        {
        }

        bool CheckCollision(sf::Vector2f point) const override { return m_Rect.contains(point); }

        sf::FloatRect GetRect() const { return m_Rect; }
        sf::Vector2f GetSize() const { return m_Rect.size; }
        sf::Vector2f GetPos() const { return m_Rect.position; }

        void SetRect(sf::FloatRect rect) { m_Rect = rect; }
        void SetSize(sf::Vector2f size) { m_Rect.size = size; }
        void SetPos(sf::Vector2f position) { m_Rect.position = position; }

    private:
        sf::FloatRect m_Rect;
    };


    class CircleCollision
        : public ICollisionStrategy
    {
    public:
        CircleCollision() = default;
        CircleCollision(sf::Vector2f position, float radius = 1.0f)
            : m_Position(position),
              m_Radius(radius) { }

        bool CheckCollision(sf::Vector2f point) const override { return (point - m_Position).lengthSquared() <= m_Radius * m_Radius; }

        float GetRadius() const { return m_Radius; }
        void SetRadius(float radius) { m_Radius = radius; }

        sf::Vector2f GetPos() const { return m_Position; }
        void SetPos(sf::Vector2f position) { m_Position = position; }

    private:
        sf::Vector2f m_Position;
        float m_Radius = 1.0f;
    };

} // CW

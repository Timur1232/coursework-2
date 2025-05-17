#pragma once

#include "pch.h"

namespace CW {

    class LineShape
        : public sf::Drawable
    {
    public:
        LineShape();
        LineShape(sf::Vector2f p1, sf::Vector2f p2);

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        sf::Vector2f GetP1Pos() const { return m_Points[0].position; }
        sf::Vector2f GetP2Pos() const { return m_Points[1].position; }

        void SetPoint1(sf::Vector2f p1);
        void SetPoint2(sf::Vector2f p2);

        // Относительно первой точки
        void SetLength(float length);

        void SetRotationByPoint1(sf::Angle angle);
        void SetRotationByPoint2(sf::Angle angle);

        void RotateByPoint1(sf::Angle angle);
        void RotateByPoint2(sf::Angle angle);

        void SetPoint1Color(sf::Color color);
        void SetPoint2Color(sf::Color color);
        void SetColor(sf::Color color);

        // Относительно первой точки
        void SetPosition(sf::Vector2f position);
        
    private:
        std::array<sf::Vertex, 2> m_Points;
    };

} // CW
#pragma once

#include "pch.h"

namespace CW {

    class LineShape
        : public sf::Drawable
    {
    public:
        LineShape();
        LineShape(float length);
        LineShape(sf::Vector2f p1, sf::Vector2f p2);

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        void SetPoint1(sf::Vector2f p1);
        void SetPoint2(sf::Vector2f p2);

        // Относительно первой точки
        void SetLength(float length);

        void RotateByPoint1(sf::Angle angle);
        void RotateByPoint2(sf::Angle angle);

        // Относительно первой точки
        void SetRotation(sf::Angle angle);

        void SetFillColor(sf::Color color);
        void SetPosition(sf::Vector2f position);

        void SetThickness(float thickness);
        
    private:
        sf::Vector2f m_Point1;
        sf::Vector2f m_Point2;
        sf::RectangleShape m_Mesh;
    };

} // CW
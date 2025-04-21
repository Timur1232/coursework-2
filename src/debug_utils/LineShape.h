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

        void setPoint1(sf::Vector2f p1);
        void setPoint2(sf::Vector2f p2);

        // Относительно первой точки
        void setLength(float length);

        void rotateByPoint1(sf::Angle angle);
        void rotateByPoint2(sf::Angle angle);

        // Относительно первой точки
        void setRotation(sf::Angle angle);

        void setFillColor(sf::Color color);
        void setPosition(sf::Vector2f position);
        
    private:
        sf::Vector2f m_Point1;
        sf::Vector2f m_Point2;
        sf::RectangleShape m_Mesh;
    };

} // CW
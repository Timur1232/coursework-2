#include "pch.h"
#include "LineShape.h"

#include "utils/utils.h"

namespace CW {

    LineShape::LineShape()
        : LineShape(1.0f)
    {
    }

    LineShape::LineShape(float length)
        : m_Mesh({length, 1.0f}), m_Point1(-length / 2.0f, 0.0f), m_Point2(length / 2.0f, 0.0f)
    {
        sf::Vector2f center = m_Mesh.getGeometricCenter();
        m_Mesh.setOrigin({center.x - length / 2.0f, center.y});
    }

    LineShape::LineShape(sf::Vector2f p1, sf::Vector2f p2)
        : m_Point1(p1), m_Point2(p2)
    {
        m_Mesh.setOrigin(m_Mesh.getGeometricCenter());

        float dist = distance(p1, p2);
        m_Mesh.setSize({ dist, m_Mesh.getSize().y });

        sf::Angle angle = (p2 - p1).angle();
        m_Mesh.setRotation(angle);
    }

    void LineShape::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_Mesh, states);
    }

    void LineShape::SetPoint1(sf::Vector2f p1)
    {
        p1 *= -1.0f;
        if (p1 == m_Point2)
            return;
        sf::Angle delta = (m_Point2 - m_Point1).angleTo(m_Point2 - p1);
        m_Mesh.rotate(delta);

        m_Point1 = p1;
        float dist = distance(m_Point1, m_Point2);
        m_Mesh.setSize({ dist, m_Mesh.getSize().y });
    }

    void LineShape::SetPoint2(sf::Vector2f p2)
    {
        p2 *= -1.0f;
        if (p2 == m_Point1)
            return;
        sf::Angle delta = (m_Point2 - m_Point1).angleTo(p2 - m_Point1);
        m_Mesh.rotate(delta);
        m_Mesh.move({ m_Point2 - p2 });

        m_Point2 = p2;
        float dist = distance(m_Point1, m_Point2);
        m_Mesh.setSize({ dist, m_Mesh.getSize().y });
    }

    void LineShape::SetLength(float length)
    {
        m_Mesh.setSize({ length, m_Mesh.getSize().y });
        sf::Vector2f delta = (m_Point2 - m_Point1).normalized() * length;
        m_Point2 = m_Point1 + delta;
    }

    void LineShape::RotateByPoint1(sf::Angle angle)
    {
        sf::Vector2f delta = (m_Point2 - m_Point1).rotatedBy(angle);
        m_Point2 = m_Point1 + delta;
        m_Mesh.rotate(angle);
    }

    void LineShape::RotateByPoint2(sf::Angle angle)
    {
        sf::Vector2f delta = (m_Point1 - m_Point2).rotatedBy(angle);
        m_Point1 = m_Point2 + delta;
        m_Mesh.rotate(angle);
    }

    void LineShape::SetRotation(sf::Angle angle)
    {
        m_Mesh.setRotation(angle);
        sf::Angle originalAngle = (m_Point2 - m_Point1).angle();
        sf::Vector2f delta = (m_Point2 - m_Point1).rotatedBy(originalAngle - angle);
        m_Point1 = m_Point2 + delta;
    }

    void LineShape::SetFillColor(sf::Color color)
    {
        m_Mesh.setFillColor(color);
    }

    void LineShape::SetPosition(sf::Vector2f position)
    {
        sf::Vector2f delta = position - m_Mesh.getPosition();
        m_Mesh.setPosition(position);
        m_Point1 += delta;
        m_Point2 += delta;
    }

    void LineShape::SetThickness(float thickness)
    {
        m_Mesh.setSize({ m_Mesh.getSize().x, thickness });
    }

} // CW
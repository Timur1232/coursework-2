#include "pch.h"
#include "LineShape.h"

#include "utils/utils.h"

namespace CW {

    LineShape::LineShape()
        : LineShape({ 0.0f, 0.0f }, {1.0f, 0.0f})
    {
    }

    LineShape::LineShape(sf::Vector2f p1, sf::Vector2f p2)
    {
        m_Points[0].position = p1;
        m_Points[1].position = p2;
    }

    void LineShape::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_Points.data(), m_Points.size(), sf::PrimitiveType::Lines);
    }

    void LineShape::SetPoint1(sf::Vector2f p1)
    {
        m_Points[0].position = p1;
    }

    void LineShape::SetPoint2(sf::Vector2f p2)
    {
        m_Points[1].position = p2;
    }

    void LineShape::SetLength(float length)
    {
        sf::Vector2f normDir = (m_Points[1].position - m_Points[0].position).normalized();
        m_Points[1].position = m_Points[0].position + normDir * length;
    }

    void LineShape::SetRotationByPoint1(sf::Angle angle)
    {
        sf::Vector2f zeroAngleSameLength = sf::Vector2f(1.0f, 0.0f) * (m_Points[1].position - m_Points[0].position).length();
        m_Points[1].position = m_Points[0].position + zeroAngleSameLength.rotatedBy(angle);
    }

    void LineShape::SetRotationByPoint2(sf::Angle angle)
    {
        sf::Vector2f zeroAngleSameLength = sf::Vector2f(1.0f, 0.0f) * (m_Points[0].position - m_Points[1].position).length();
        m_Points[0].position = m_Points[1].position + zeroAngleSameLength.rotatedBy(angle);
    }

    void LineShape::RotateByPoint1(sf::Angle angle)
    {
        sf::Vector2f delta = (m_Points[1].position - m_Points[0].position).rotatedBy(angle);
        m_Points[1].position = m_Points[0].position + delta;
    }

    void LineShape::RotateByPoint2(sf::Angle angle)
    {
        sf::Vector2f delta = (m_Points[0].position - m_Points[1].position).rotatedBy(angle);
        m_Points[0].position = m_Points[1].position + delta;
    }

    void LineShape::SetPoint1Color(sf::Color color)
    {
        m_Points[0].color = color;
    }

    void LineShape::SetPoint2Color(sf::Color color)
    {
        m_Points[1].color = color;
    }

    void LineShape::SetColor(sf::Color color)
    {
        SetPoint1Color(color);
        SetPoint2Color(color);
    }

    void LineShape::SetPosition(sf::Vector2f position)
    {
        sf::Vector2f delta = m_Points[1].position - m_Points[0].position;
        m_Points[0].position = position;
        m_Points[1].position = m_Points[0].position + delta;
    }

} // CW
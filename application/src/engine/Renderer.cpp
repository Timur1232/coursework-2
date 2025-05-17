#include "pch.h"
#include "Renderer.h"

namespace CW {

    Renderer& Renderer::Get()
    {
        static Renderer renderer;
        return renderer;
    }

    void Renderer::Draw(const sf::Drawable& target, const sf::RenderStates& states)
    {
        m_RenderTarget->draw(target, states);
    }

    RCircleShapeBuilder& CW::RCircleShapeBuilder::Draw()
    {
        Renderer::Get().Draw(m_CirlceMesh, m_Shader);
        if (m_Default)
            SetDefault();
        return *this;
    }

    RCircleShapeBuilder& RCircleShapeBuilder::SetDefault()
    {
        Position({0.0f, 0.0f})
            .Radius(10.0f)
            .Color(sf::Color::White)
            .Rotation(sf::Angle::Zero)
            .Scale({ 1.0f, 1.0f })
            .OutlineColor(sf::Color::White)
            .OutlineThickness(0.0f)
            .PointCount(16)
            .Texture(nullptr, true)
            .Shader(nullptr);
        m_Default = false;
        return *this;
    }

    RRectangleShapeBuilder& RRectangleShapeBuilder::SetDefault()
    {
        Position({ 0.0f, 0.0f })
            .Color(sf::Color::White)
            .Rotation(sf::Angle::Zero)
            .Size({10.0f, 10.0f})
            .Scale({ 1.0f, 1.0f })
            .OutlineColor(sf::Color::White)
            .OutlineThickness(1.0f)
            .Texture(nullptr, true)
            .Shader(nullptr);
        m_Default = false;
        return *this;
    }

    RRectangleShapeBuilder& CW::RRectangleShapeBuilder::Draw()
    {
        Renderer::Get().Draw(m_RectangleMesh, m_Shader);
        if (m_Default)
            SetDefault();
        return *this;
    }

    RLineShapeBuilder& RLineShapeBuilder::SetDefault()
    {
        P1Position({ 0.0f, 0.0f })
            .P2Position({ 1.0f, 0.0f })
            .Color(sf::Color::White);
        m_Default = false;
        return *this;
    }

    RLineShapeBuilder& RLineShapeBuilder::Draw()
    {
        Renderer::Get().Draw(m_LineMesh);
        if (m_Default)
            SetDefault();
        return *this;
    }

    RConvexShapeSetter& RConvexShapeSetter::Draw()
    {
        Renderer::Get().Draw(m_ConvexMesh, m_Shader);
        return *this;
    }

    RDotBuilder& RDotBuilder::SetDefault()
    {
        Position({ 0.0f, 0.0f })
            .Color(sf::Color::White);
        m_Default = false;
        return *this;
    }

    RDotBuilder& RDotBuilder::Draw()
    {
        Renderer::Get().Draw(m_Dot);
        if (m_Default)
            SetDefault();
        return *this;
    }

} // CW
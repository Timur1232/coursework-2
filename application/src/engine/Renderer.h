#pragma once

#include "pch.h"
#include "debug_utils/LineShape.h"
#include "debug_utils/DotShape.h"

namespace CW {

    class RCircleShapeBuilder
    {
    public:
        using Self = RCircleShapeBuilder;
    public:
        RCircleShapeBuilder() { SetDefault(); }

        Self& SetDefault();
        Self& DefaultAfterDraw() { m_Default = true; return *this; }

        Self& Position(sf::Vector2f position) { m_CirlceMesh.setPosition(position); return *this; }
        Self& Radius(float radius) { m_CirlceMesh.setRadius(radius); m_CirlceMesh.setOrigin(m_CirlceMesh.getGeometricCenter()); return *this; }
        Self& Color(sf::Color color) { m_CirlceMesh.setFillColor(color); return *this; }
        Self& Rotation(sf::Angle angle) { m_CirlceMesh.setRotation(angle); return *this; }
        Self& Scale(sf::Vector2f scale) { m_CirlceMesh.setScale(scale); return *this; }
        Self& OutlineColor(sf::Color color) { m_CirlceMesh.setOutlineColor(color); return *this; }
        Self& OutlineThickness(float thickness) { m_CirlceMesh.setOutlineThickness(thickness); return *this; }
        Self& PointCount(size_t count) { m_CirlceMesh.setPointCount(count); return *this; }
        Self& Texture(const sf::Texture* texture, bool resetRect = false) { m_CirlceMesh.setTexture(texture, resetRect); return *this; }
        Self& TextureRect(sf::IntRect rect) { m_CirlceMesh.setTextureRect(rect); return *this; }
        Self& Shader(const sf::Shader* shader) { m_Shader = shader; return *this; }

        Self& Draw();

    private:
        sf::CircleShape m_CirlceMesh;
        bool m_Default = false;
        const sf::Shader* m_Shader = nullptr;
    };

    class RRectangleShapeBuilder
    {
    public:
        using Self = RRectangleShapeBuilder;
    public:
        RRectangleShapeBuilder() { SetDefault(); }

        Self& SetDefault();
        Self& DefaultAfterDraw() { m_Default = true; return *this; }

        Self& Position(sf::Vector2f position) { m_RectangleMesh.setPosition(position); return *this; }
        Self& Color(sf::Color color) { m_RectangleMesh.setFillColor(color); return *this; }
        Self& Rotation(sf::Angle angle) { m_RectangleMesh.setRotation(angle); return *this; }
        Self& Size(sf::Vector2f size) { m_RectangleMesh.setSize(size); return *this; }
        Self& Scale(sf::Vector2f scale) { m_RectangleMesh.setScale(scale); return *this; }
        Self& OutlineColor(sf::Color color) { m_RectangleMesh.setOutlineColor(color); return *this; }
        Self& OutlineThickness(float thickness) { m_RectangleMesh.setOutlineThickness(thickness); return *this; }
        Self& Texture(const sf::Texture* texture, bool resetRect = false) { m_RectangleMesh.setTexture(texture, resetRect); return *this; }
        Self& TextureRect(sf::IntRect rect) { m_RectangleMesh.setTextureRect(rect); return *this; }
        Self& Shader(const sf::Shader* shader) { m_Shader = shader; return *this; }

        Self& Draw();

    private:
        sf::RectangleShape m_RectangleMesh;
        bool m_Default = false;
        const sf::Shader* m_Shader = nullptr;
    };

    class RLineShapeBuilder
    {
    public:
        using Self = RLineShapeBuilder;
    public:
        RLineShapeBuilder() { SetDefault(); }

        Self& SetDefault();
        Self& DefaultAfterDraw() { m_Default = true; return *this; }

        Self& P1Position(sf::Vector2f position) { m_LineMesh.SetPoint1(position); return *this; }
        Self& P2Position(sf::Vector2f position) { m_LineMesh.SetPoint2(position); return *this; }
        Self& Position(sf::Vector2f position) { m_LineMesh.SetPosition(position); return *this; }
        Self& Color(sf::Color color) { m_LineMesh.SetColor(color); return *this; }
        Self& P1Color(sf::Color color) { m_LineMesh.SetPoint1Color(color); return *this; }
        Self& P2Color(sf::Color color) { m_LineMesh.SetPoint2Color(color); return *this; }
        Self& SetRotationByP1(sf::Angle angle) { m_LineMesh.SetRotationByPoint1(angle); return *this; }
        Self& SetRotationByP2(sf::Angle angle) { m_LineMesh.SetRotationByPoint2(angle); return *this; }
        Self& RotateByP1(sf::Angle angle) { m_LineMesh.RotateByPoint1(angle); return *this; }
        Self& RotateByP2(sf::Angle angle) { m_LineMesh.RotateByPoint2(angle); return *this; }
        Self& Length(float length) { m_LineMesh.SetLength(length); return *this; }

        Self& Draw();

    private:
        LineShape m_LineMesh;
        bool m_Default = false;
    };

    class RConvexShapeSetter
    {
    public:
        using Self = RConvexShapeSetter;
    public:
        RConvexShapeSetter(sf::ConvexShape& mesh)
            : m_ConvexMesh(mesh) { }

        Self& Position(sf::Vector2f position) { m_ConvexMesh.setPosition(position); return *this; }
        Self& Color(sf::Color color) { m_ConvexMesh.setFillColor(color); return *this; }
        Self& Rotation(sf::Angle angle) { m_ConvexMesh.setRotation(angle); return *this; }
        Self& Scale(sf::Vector2f scale) { m_ConvexMesh.setScale(scale); return *this; }
        Self& OutlineColor(sf::Color color) { m_ConvexMesh.setOutlineColor(color); return *this; }
        Self& OutlineThickness(float thickness) { m_ConvexMesh.setOutlineThickness(thickness); return *this; }
        Self& Texture(const sf::Texture* texture, bool resetRect = false) { m_ConvexMesh.setTexture(texture, resetRect); return *this; }
        Self& TextureRect(sf::IntRect rect) { m_ConvexMesh.setTextureRect(rect); return *this; }
        Self& Shader(const sf::Shader* shader) { m_Shader = shader; return *this; }

        Self& Draw();

    private:
        sf::ConvexShape& m_ConvexMesh;
        const sf::Shader* m_Shader = nullptr;
    };

    class RDotBuilder
    {
    public:
        using Self = RDotBuilder;
    public:
        RDotBuilder() { SetDefault(); }

        Self& SetDefault();
        Self& DefaultAfterDraw() { m_Default = true; return *this; }

        Self& Position(sf::Vector2f position) { m_Dot.SetPosition(position); return *this; }
        Self& Color(sf::Color color) { m_Dot.SetColor(color); return *this; }

        Self& Draw();

    private:
        DotShape m_Dot;
        bool m_Default = false;
    };


    class Renderer
    {
    public:
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;

        static Renderer& Get();
        void SetRenderTarget(Shared<sf::RenderTarget> target) { m_RenderTarget = target; }
        void ReleaseRenderTarget() { m_RenderTarget.reset(); }
        void Draw(const sf::Drawable& target, const sf::RenderStates& states = sf::RenderStates::Default);
        void SetView(const sf::View& view) { m_RenderTarget->setView(view); }
        void SetDefaultView() { m_RenderTarget->setView(m_RenderTarget->getDefaultView()); }

        RCircleShapeBuilder& BeginCircleShape() { return m_CircleShapeBuilde; }
        RRectangleShapeBuilder& BeginRectangleShape() { return m_RectangleShapeBuilder; }
        RLineShapeBuilder& BeginLineShape() { return m_LineShapeBuilder; }
        RConvexShapeSetter BeginConvexShapeSetting(sf::ConvexShape& mesh) { return RConvexShapeSetter(mesh); }
        RDotBuilder& BeginDotShape() { return m_DotBuilder; }

    private:
        Renderer() = default;

    private:
        Shared<sf::RenderTarget> m_RenderTarget;

        RCircleShapeBuilder m_CircleShapeBuilde;
        RRectangleShapeBuilder m_RectangleShapeBuilder;
        RLineShapeBuilder m_LineShapeBuilder;
        RDotBuilder m_DotBuilder;
    };

} // CW
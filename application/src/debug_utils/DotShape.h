#pragma once
#include "pch.h"

namespace CW {

	class DotShape
		: public sf::Drawable
	{
	public:
		DotShape() = default;
		DotShape(sf::Vector2f position, sf::Color color = sf::Color::White)
		{
			m_DotVertex.position = position;
			m_DotVertex.color = color;
		}

		void SetPosition(sf::Vector2f position) { m_DotVertex.position = position; }
		void SetColor(sf::Color color) { m_DotVertex.color = color; }

		sf::Vector2f GetPosition() const { return m_DotVertex.position; }
		sf::Color GetColor() const { return m_DotVertex.color; }

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			target.draw(&m_DotVertex, 1, sf::PrimitiveType::Points);
		}

	private:
		sf::Vertex m_DotVertex;
	};

} // CW
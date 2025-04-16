#pragma once

#include <SFML/Graphics.hpp>

#include "engine/IDrawable.h"

namespace CW {

	class Resource
		: public IDrawable
	{
	public:
		Resource(sf::Vector2f position);

		static void staticInit();

		inline sf::Vector2f getPos() const { return m_Position; }
		inline int getResources() const { return m_Amount; }

		void draw(sf::RenderWindow& render) const override;

	private:
		sf::Vector2f m_Position;
		int m_Amount = 10;

		static sf::CircleShape s_Mesh;
	};

} // Cw
#pragma once

#include <SFML/Graphics.hpp>

#include "engine/IDrawable.h"
#include "engine/IUpdate.h"

namespace CW {

	class ResourceReciever
		: public IDrawable
	{
	public:
		ResourceReciever(sf::Vector2f position);

		void draw(sf::RenderWindow& render) const override;

		inline int getResources() const;
		inline sf::Vector2f getPos() const;
		inline float getBroadcastRadius() const;

		inline void addResources(int amount);
		inline int takeResources(int amount);

	private:
		sf::Vector2f m_Position;
		int m_ResourceCount = 0;
		float m_RecieveRadius;
		float m_BroadcastRadius;
		sf::CircleShape m_Mesh;
	};

} // CW
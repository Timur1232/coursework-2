#pragma once

#include <SFML/Graphics.hpp>

#include "engine/IDrawable.h"

namespace CW {

	class ResourceReciever
		: public IDrawable
	{
	public:
		ResourceReciever(sf::Vector2f position);

		void debugInterface();

		void draw(sf::RenderWindow& render) const override;

		int getResources() const;
		sf::Vector2f getPos() const;
		float getBroadcastRadius() const;
		float getRecieveRadius() const;

		void addResources(int amount);
		int takeResources(int amount);

	private:
		sf::Vector2f m_Position;
		int m_ResourceCount = 0;
		float m_RecieveRadius;
		float m_BroadcastRadius;

		sf::CircleShape m_Mesh;
		sf::CircleShape m_RecieveMesh;
		sf::CircleShape m_BroadcastMesh;
	};

} // CW
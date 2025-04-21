#pragma once

#include "pch.h"

#include "engine/IDrawable.h"

namespace CW {

	class ResourceReciever
		: public IDrawable
	{
	public:
		ResourceReciever(sf::Vector2f position);

		void DebugInterface();

		void Draw(sf::RenderWindow& render) const override;

		int GetResources() const;
		sf::Vector2f GetPos() const;
		float GetBroadcastRadius() const;
		float GetRecieveRadius() const;

		void AddResources(int amount);
		int TakeResources(int amount);

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
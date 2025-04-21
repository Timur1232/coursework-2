#pragma once

#include "pch.h"

#include "engine/IDrawable.h"

namespace CW {

	class Resource
		: public IDrawable
	{
	public:
		Resource(sf::Vector2f position, int amount = 10);

		static void StaticInit();

		inline sf::Vector2f GetPos() const { return m_Position; }
		inline int GetResources() const { return m_Amount; }
		inline bool IsCarried() const { return m_IsCarried; }
		void PickUp();

		void Draw(sf::RenderWindow& render) const override;

	private:
		sf::Vector2f m_Position;
		int m_Amount = 10;
		bool m_IsCarried = false;

		static sf::CircleShape s_Mesh;
	};

} // Cw
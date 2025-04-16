#include "Resource.h"

namespace CW {

	Resource::Resource(sf::Vector2f position)
		: m_Position(position)
	{
	}

	void Resource::staticInit()
	{
		s_Mesh.setRadius(20.0f);
		s_Mesh.setOrigin({ s_Mesh.getRadius(), s_Mesh.getRadius() });
		s_Mesh.setFillColor(sf::Color::Cyan);
	}

	void Resource::draw(sf::RenderWindow& render) const
	{
		s_Mesh.setPosition(m_Position);
		render.draw(s_Mesh);
	}

} // CW
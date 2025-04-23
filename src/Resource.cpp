#include "pch.h"
#include "Resource.h"

#include "debug_utils/Log.h"

namespace CW {

	sf::CircleShape Resource::s_Mesh;

	Resource::Resource(sf::Vector2f position, int amount)
		: Object(position), m_Amount(amount)
	{
	}

	void Resource::StaticInit()
	{
		s_Mesh.setRadius(20.0f);
		s_Mesh.setOrigin({ s_Mesh.getRadius(), s_Mesh.getRadius() });
		s_Mesh.setFillColor(sf::Color::Cyan);
	}

	void Resource::PickUp()
	{
		if (m_IsCarried)
		{
			CW_WARN("Resource on position ({:.2}, {:.2}) already picked!", m_Position.x, m_Position.y);
		}
		m_IsCarried = true;
	}

	void Resource::Draw(sf::RenderWindow& render) const
	{
		if (!IsCarried())
		{
			s_Mesh.setPosition(m_Position);
			render.draw(s_Mesh);
		}
	}

} // CW
#include "ResourceReciever.h"

namespace CW {

	ResourceReciever::ResourceReciever(sf::Vector2f position)
		: m_Position(position)
	{
		m_RecieveRadius = 300.0f;
		m_RecieveRadius = 1000.0f;

		m_Mesh.setPosition(m_Position);
		m_Mesh.setRadius(100.0f);
		m_Mesh.setOrigin({ m_Mesh.getRadius(), m_Mesh.getRadius() });
		m_Mesh.setFillColor(sf::Color::Magenta);
	}

	void ResourceReciever::draw(sf::RenderWindow& render) const
	{
	}

	inline int ResourceReciever::getResources() const
	{
		return m_ResourceCount;
	}

	inline sf::Vector2f ResourceReciever::getPos() const
	{
		return m_Position;
	}

	inline float ResourceReciever::getBroadcastRadius() const
	{
		return m_BroadcastRadius;
	}

	inline void ResourceReciever::addResources(int amount)
	{
		m_ResourceCount += amount;
	}

	inline int ResourceReciever::takeResources(int amount)
	{
		int take;
		if (amount > m_ResourceCount)
		{
			take = m_ResourceCount;
			m_ResourceCount = 0;
		}
		else
		{
			take = amount;
			m_ResourceCount -= amount;
		}
		return take;
	}

} // CW
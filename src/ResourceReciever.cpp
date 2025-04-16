#include "ResourceReciever.h"

#include <imgui.h>

#include "debug_utils/Log.h"

namespace CW {

	ResourceReciever::ResourceReciever(sf::Vector2f position)
		: m_Position(position), m_RecieveRadius(300.f), m_BroadcastRadius(1000.f),
		  m_Mesh(100.f)
	{
		m_Mesh.setOrigin({ m_Mesh.getRadius(), m_Mesh.getRadius() });
		m_Mesh.setPosition(m_Position);
		m_Mesh.setFillColor(sf::Color::Magenta);

		m_RecieveMesh.setRadius(m_RecieveRadius);
		m_RecieveMesh.setOrigin({ m_RecieveRadius, m_RecieveRadius });
		m_RecieveMesh.setPosition(m_Position);
		m_RecieveMesh.setFillColor(sf::Color::Transparent);
		m_RecieveMesh.setOutlineColor(sf::Color::Green);
		m_RecieveMesh.setOutlineThickness(1.0f);

		m_BroadcastMesh.setRadius(m_BroadcastRadius);
		m_BroadcastMesh.setOrigin({ m_BroadcastRadius, m_BroadcastRadius });
		m_BroadcastMesh.setPosition(m_Position);
		m_BroadcastMesh.setFillColor(sf::Color::Transparent);
		m_BroadcastMesh.setOutlineColor(sf::Color::Red);
		m_BroadcastMesh.setOutlineThickness(1.0f);
	}

	void ResourceReciever::debugInterface()
	{
		ImGui::Text("resources amount: %d", m_ResourceCount);
	}

	void ResourceReciever::draw(sf::RenderWindow& render) const
	{
		render.draw(m_Mesh);
		render.draw(m_RecieveMesh);
		render.draw(m_BroadcastMesh);
	}

	int ResourceReciever::getResources() const
	{
		return m_ResourceCount;
	}

	sf::Vector2f ResourceReciever::getPos() const
	{
		return m_Position;
	}

	float ResourceReciever::getBroadcastRadius() const
	{
		return m_BroadcastRadius;
	}

	float ResourceReciever::getRecieveRadius() const
	{
		return m_RecieveRadius;
	}

	void ResourceReciever::addResources(int amount)
	{
		CW_TRASE("Added {} resources", amount);
		m_ResourceCount += amount;
	}

	int ResourceReciever::takeResources(int amount)
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
#include "pch.h"
#include "ResourceReciever.h"

#include "debug_utils/Log.h"
#include "engine/Renderer.h"

namespace CW {

	ResourceReciever::ResourceReciever(sf::Vector2f position)
		: Object(position), m_RecieveRadius(300.f), m_BroadcastRadius(1000.f)
	{
	}

	void ResourceReciever::DebugInterface() const
	{
		ImGui::Text("resources amount: %d", m_ResourceCount);
	}

	void ResourceReciever::Draw(sf::RenderWindow& render)
	{
		auto& circleBuilder = Renderer::Get().BeginCircleShape();
		// body
		circleBuilder.DefaultAfterDraw()
			.Radius(100.0f)
			.Position(m_Position)
			.Color(sf::Color::Magenta)
			.Draw();
		// recieve radius
		circleBuilder.DefaultAfterDraw()
			.Radius(m_RecieveRadius)
			.Position(m_Position)
			.Color(sf::Color::Transparent)
			.OutlineThickness(1.0f)
			.OutlineColor(sf::Color::Green)
			.Draw();
		// broadcast radius
		circleBuilder.DefaultAfterDraw()
			.Radius(m_BroadcastRadius)
			.Position(m_Position)
			.Color(sf::Color::Transparent)
			.OutlineThickness(1.0f)
			.OutlineColor(sf::Color::Red)
			.Draw();
	}

	int ResourceReciever::GetResources() const
	{
		return m_ResourceCount;
	}

	float ResourceReciever::GetBroadcastRadius() const
	{
		return m_BroadcastRadius;
	}

	float ResourceReciever::GetRecieveRadius() const
	{
		return m_RecieveRadius;
	}

	void ResourceReciever::AddResources(int amount)
	{
		CW_TRACE("Added {} resources", amount);
		m_ResourceCount += amount;
	}

	int ResourceReciever::TakeResources(int amount)
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
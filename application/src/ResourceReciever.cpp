#include "pch.h"
#include "ResourceReciever.h"

#include "debug_utils/Log.h"
#include "engine/Renderer.h"
#include "engine/Events/UserEventHandler.h"

namespace CW {

	ResourceReciever::ResourceReciever(sf::Vector2f position, int droneCost, float spawnCooldown)
		: Object(position), m_RecieveRadius(300.f), m_BroadcastRadius(1000.f),
		  m_DroneSpawnCooldown(spawnCooldown), m_DroneCost(droneCost)
	{
		m_DroneSpawnTimer = m_DroneSpawnCooldown;
	}

	ResourceReciever::ResourceReciever(const DroneSettings& settings)
		: ResourceReciever(settings.BasePosition, settings.DroneCost, settings.DroneSpawnCooldown)
	{
	}

	void ResourceReciever::DebugInterface() const
	{
		ImGui::Text("resources amount: %d", m_ResourceCount);
	}

	void ResourceReciever::Draw()
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

	void ResourceReciever::Update(float deltaTime)
	{
		m_DroneSpawnTimer -= deltaTime;
		if (m_DroneSpawnTimer <= 0.0f && m_ResourceCount >= m_DroneCost)
		{
			m_ResourceCount -= m_DroneCost;
			UserEventHandler::Get().SendEvent(SpawnDrone{ m_Position });
			m_DroneSpawnTimer = m_DroneSpawnCooldown;
			CW_TRACE("Drone spawned");
		}
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
#include "pch.h"
#include "ResourceReciever.h"

#include "debug_utils/Log.h"
#include "engine/Renderer.h"
#include "engine/Events/UserEventHandler.h"

namespace CW {

	MotherBase::MotherBase(sf::Vector2f position, int droneCost, float spawnCooldown)
		: Object(position), m_RecieveRadius(300.f), m_BroadcastRadius(1000.f),
		  m_DroneSpawnCooldown(spawnCooldown), m_DroneCost(droneCost)
	{
		m_DroneSpawnTimer = m_DroneSpawnCooldown;
	}

	MotherBase::MotherBase(const DroneSettings& settings)
		: MotherBase(settings.BasePosition, settings.DroneCost, settings.DroneSpawnCooldown)
	{
	}

	MotherBaseData MotherBase::GetData() const
	{
		return MotherBaseData{
			.ResourceCount		= m_ResourceCount,
			.RecieveRadius		= m_RecieveRadius,
			.BroadcastRadius	= m_BroadcastRadius,
			.DroneCost			= m_DroneCost,
			.DroneSpawnTimer    = m_DroneSpawnTimer,
			.DroneSpawnCooldown = m_DroneSpawnCooldown,
		};
	}

	void MotherBase::SetData(const MotherBaseData& data)
	{
		m_ResourceCount = data.ResourceCount;
		m_RecieveRadius = data.RecieveRadius;
		m_BroadcastRadius = data.BroadcastRadius;
		m_DroneCost = data.DroneCost;
		m_DroneSpawnTimer = data.DroneSpawnTimer;
		m_DroneSpawnCooldown = data.DroneSpawnCooldown;
	}

	bool MotherBase::Update(float deltaTime)
	{
		m_DroneSpawnTimer -= deltaTime;
		if (m_DroneSpawnTimer <= 0.0f && m_ResourceCount >= m_DroneCost)
		{
			m_ResourceCount -= m_DroneCost;
			m_DroneSpawnTimer = m_DroneSpawnCooldown;
			return true;
		}
		return false;
	}

	int MotherBase::GetResources() const
	{
		return m_ResourceCount;
	}

	float MotherBase::GetBroadcastRadius() const
	{
		return m_BroadcastRadius;
	}

	float MotherBase::GetRecieveRadius() const
	{
		return m_RecieveRadius;
	}

	void MotherBase::AddResources(int amount)
	{
		CW_TRACE("Added {} resources", amount);
		m_ResourceCount += amount;
		m_TotalResourceCount += amount;
	}

	int MotherBase::TakeResources(int amount)
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
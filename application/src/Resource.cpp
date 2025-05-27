#include "pch.h"
#include "Resource.h"

#include "debug_utils/Log.h"
#include "engine/Renderer.h"
#include "utils/utils.h"
#include "SimState.h"

namespace CW {

	Resource::Resource(sf::Vector2f position, sf::Angle rotation, int amount)
		: Object(position), m_Rotation(rotation), m_Amount(amount)
	{
	}

	void Resource::WriteToFile(std::ofstream& file) const
	{
		file.write(reinterpret_cast<const char*>(&m_Position), sizeof(m_Position));
		float rotationRad = m_Rotation.asRadians();
		file.write(reinterpret_cast<const char*>(&rotationRad), sizeof(rotationRad));
		file.write(reinterpret_cast<const char*>(&m_Amount), sizeof(m_Amount));
	}

	void Resource::ReadFromFile(std::ifstream& file)
	{
		file.read(reinterpret_cast<char*>(&m_Position), sizeof(m_Position));
		float rotationRad;
		file.read(reinterpret_cast<char*>(&rotationRad), sizeof(rotationRad));
		m_Rotation = sf::radians(rotationRad);
		file.read(reinterpret_cast<char*>(&m_Amount), sizeof(m_Amount));
	}

	void Resource::Pickup()
	{
		if (m_IsCarried)
		{
			CW_WARN("Resource on position ({:.2}, {:.2}) already picked!", m_Position.x, m_Position.y);
		}
		m_IsCarried = true;
	}

	void Resource::Revive(sf::Vector2f position, sf::Angle rotation, int amount)
	{
		m_Position = position;
		m_Amount = amount;
		m_Rotation = rotation;
		m_IsCarried = false;
	}


	ResourceManager::ResourceManager(const ResourcesSettings& settings)
		: m_Settings(settings)
	{
	}

	void ResourceManager::SetState(FullSimulationState& state)
	{
		m_Settings = state.Settings.Resources;
		m_Resources = std::move(state.Resources);
	}

	void ResourceManager::CollectState(SimulationState& state) const
	{
		for (const auto& resource : m_Resources)
		{
			if (!resource.IsCarried())
			{
				state.ResourcesPositions.push_back(resource.GetPos());
				state.ResourcesRotations.push_back(resource.GetRotation());
				state.ResourcesAmounts.push_back(resource.GetResources());
			}
		}
	}

	void ResourceManager::CollectState(FullSimulationState& state) const
	{
		for (const auto& resource : m_Resources)
			if (!resource.IsCarried())
				state.Resources.push_back(resource);
	}

	void ResourceManager::SetSettings(const ResourcesSettings& settings)
	{
		m_Settings = settings;
	}

	void ResourceManager::CreateResource(sf::Vector2f position, sf::Angle rotation, int amount)
	{
		auto carriedResource = std::ranges::find_if(m_Resources.begin(), m_Resources.end(), [](const Resource& r) { return r.IsCarried(); });

		if (carriedResource != m_Resources.end())
		{
			carriedResource->Revive(position, rotation, amount);
		}
		else
		{
			m_Resources.emplace_back(position, rotation, amount);
		}
	}

	void ResourceManager::GenerateResourceOnSection(const TerrainGenerator& terrain, int sectionKey)
	{
		if (rand_float() > m_Settings.GenerateChance)
			return;

		float leftX = terrain.CalcSectionStartPosition(sectionKey) + m_Settings.ClusterSize;
		float rightX = leftX + terrain.GetSectionWidth() - m_Settings.ClusterSize;

		float spawnPosX = lerp(leftX, rightX, rand_float());
		int resourceCount = (int)((m_Settings.MaxResourcesInCluster - 5) * rand_float()) + 5;
		for (int i = 0; i < resourceCount; ++i)
		{
			float xOffset = lerp(-m_Settings.ClusterSize, m_Settings.ClusterSize, rand_float());
			float posX = spawnPosX + xOffset;
			sf::Vector2f normal = terrain.GetNormal(posX);
			sf::Angle angle = normal.perpendicular().angle();

			sf::Vector2f position(posX, terrain.GetHeight(posX));
			position += normal * 50.0f;
			int amount = (int)((m_Settings.MaxResourceAmount - 5) * rand_float()) + 5;
			CreateResource(position, angle, amount);
		}
	}

} // CW
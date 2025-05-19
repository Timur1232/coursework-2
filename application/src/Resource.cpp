#include "pch.h"
#include "Resource.h"

#include "debug_utils/Log.h"
#include "engine/Renderer.h"
#include "utils/utils.h"

namespace CW {

	Resource::Resource(sf::Vector2f position, int amount)
		: Object(position), m_Amount(amount)
	{
	}

	void Resource::Pickup()
	{
		if (m_IsCarried)
		{
			CW_WARN("Resource on position ({:.2}, {:.2}) already picked!", m_Position.x, m_Position.y);
		}
		m_IsCarried = true;
	}

	void Resource::Revive(sf::Vector2f position, int amount)
	{
		m_Position = position;
		m_Amount = amount;
		m_IsCarried = false;
	}


	ResourceManager::ResourceManager(const ResourcesSettings& settings)
		: m_Settings(settings)
	{
	}

	void ResourceManager::SetSettings(const ResourcesSettings& settings)
	{
		m_Settings = settings;
	}

	void ResourceManager::DrawAllRecources()
	{
		auto validResources = m_Resources
			| std::ranges::views::filter([](const Resource& r) { return !r.IsCarried(); });
		
		for (const auto& resource : validResources)
		{
			Renderer::Get().BeginCircleShape()
				.DefaultAfterDraw()
				.Radius(20.0f)
				.Color(sf::Color::Cyan)
				.Position(resource.GetPos())
				.Draw();
		}
	}

	void ResourceManager::CreateResource(sf::Vector2f position, int amount)
	{
		auto carriedResource = std::ranges::find_if(m_Resources.begin(), m_Resources.end(), [](const Resource& r) { return r.IsCarried(); });

		if (carriedResource != m_Resources.end())
		{
			carriedResource->Revive(position, amount);
		}
		else
		{
			m_Resources.emplace_back(position, amount);
		}
	}

	void ResourceManager::GenerateResourceOnSection(const Terrain& terrain, int sectionKey)
	{
		if (rand_float() > m_Settings.GenerateChance)
			return;

		const auto& generator = terrain.GetNoiseGenerator();

		float leftX = terrain.CalcSectionStartPosition(sectionKey);
		float rightX = leftX + terrain.GetSectionWidth();

		float spawnPosX = lerp(leftX, rightX, rand_float());
		float spawnPosY = terrain.GetHeight(spawnPosX) - m_Settings.ClusterSize - 100.0f;
		sf::Vector2f spawnPos(spawnPosX, spawnPosY);

		int resourceCount = m_Settings.MaxResourcesInCluster * rand_float();
		for (int i = 0; i < resourceCount; ++i)
		{
			sf::Angle angle = lerp(sf::degrees(-180.0f), sf::degrees(180.0f), rand_float());
			float dist = m_Settings.ClusterSize * rand_float();

			sf::Vector2f position = spawnPos + ONE_LENGTH_VEC.rotatedBy(angle) * dist;
			int amount = m_Settings.MaxResourceAmount * rand_float();
			m_Resources.emplace_back(position, amount);
		}
	}

} // CW
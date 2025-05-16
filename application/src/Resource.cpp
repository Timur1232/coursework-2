#include "pch.h"
#include "Resource.h"

#include "debug_utils/Log.h"
#include "engine/Renderer.h"

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


	void ResourceManager::DrawAllRecources(sf::RenderWindow& render)
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

} // CW
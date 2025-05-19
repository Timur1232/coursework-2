#pragma once

#include "pch.h"

#include "engine/IDrawable.h"
#include "engine/Object.h"

#include "utils/utils.h"
#include "Terrain.h"

#include "ResourcesSettings.h"

namespace CW {

	class Resource
		: public Object
	{
	public:
		Resource(sf::Vector2f position, int amount = 10);

		inline int GetResources() const { return m_Amount; }
		inline bool IsCarried() const { return m_IsCarried; }

		void Pickup();
		void Revive(sf::Vector2f position, int amount = 10);

	private:
		int m_Amount = 10;
		bool m_IsCarried = false;
	};


	class ResourceManager
	{
	public:
		ResourceManager() = default;
		ResourceManager(const ResourcesSettings& settings);

		void SetSettings(const ResourcesSettings& settings);

		void DrawAllRecources();

		void Clear() { m_Resources.clear(); }
		void Reserve(size_t reserve) { m_Resources.reserve(reserve); }

		void CreateResource(sf::Vector2f position, int amount = 10);

		std::vector<Resource>& GetResources() { return m_Resources; }

		void GenerateResourceOnSection(const Terrain& terrain, int sectionKey);

	private:
		std::vector<Resource> m_Resources;
		ResourcesSettings m_Settings;
	};

} // CW
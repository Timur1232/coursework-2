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
		Resource() = default;
		Resource(sf::Vector2f position, sf::Angle rotation, int amount = 10);

		void WriteToFile(std::ofstream& file) const;
		void ReadFromFile(std::ifstream& file);

		inline int GetResources() const { return m_Amount; }
		inline bool IsCarried() const { return m_IsCarried; }

		sf::Angle GetRotation() const { return m_Rotation; }

		void Pickup();
		void Revive(sf::Vector2f position, sf::Angle rotation, int amount = 10);

	private:
		sf::Angle m_Rotation = sf::Angle::Zero;
		int m_Amount = 10;
		bool m_IsCarried = false;
	};


	struct SimulationState;
	struct FullSimulationState;

	class ResourceManager
	{
	public:
		ResourceManager() = default;
		ResourceManager(const ResourcesSettings& settings);
		
		void SetState(FullSimulationState& state);

		void CollectState(SimulationState& state) const;
		void CollectState(FullSimulationState& state) const;

		void SetSettings(const ResourcesSettings& settings);

		void Clear() { m_Resources.clear(); }
		void Reserve(size_t reserve) { m_Resources.reserve(reserve); }

		void CreateResource(sf::Vector2f position, sf::Angle rotation, int amount = 10);

		std::vector<Resource>& GetResources() { return m_Resources; }

		void GenerateResourceOnSection(const TerrainGenerator& terrain, int sectionKey);

	private:
		std::vector<Resource> m_Resources;
		ResourcesSettings m_Settings;
	};

} // CW
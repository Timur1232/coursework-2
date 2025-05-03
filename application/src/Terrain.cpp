#include "pch.h"
#include "Terrain.h"

#include "debug_utils/Log.h"

namespace CW {

	TerrainSection::TerrainSection(int index)
		: Index(index)
	{
		Samples.fill(0);
	}

	void TerrainSection::Generate(NoiseGenerator& gen, int seed, float maxHeight, float mapNoiseDistance)
	{
		float noiseX = static_cast<float>(Index) * mapNoiseDistance;
		float noiseStep = mapNoiseDistance / static_cast<float>(TERRAIN_SAMPLES_AMOUNT);

		for (auto& sample : Samples)
		{
			float noiseValue = gen->GenSingle2D(noiseX, 0.0f, seed);
			sample = noiseValue * maxHeight;
			noiseX += noiseStep;
		}
	}

	Terrain::Terrain()
		: m_NoiseGenerator(FastNoise::New<FastNoise::Perlin>())
	{
		m_DotMesh.setOrigin(m_DotMesh.getGeometricCenter());
	}

	void Terrain::Generate(int index)
	{
		auto found = std::find_if(m_TerrainSections.begin(), m_TerrainSections.end(),
			[index](const TerrainSection& section) {
				return section.Index == index;
			});
		if (found != m_TerrainSections.end())
		{
			CW_WARN("Trying to generate existing terrain section on position: {}", index);
			return;
		}

		m_TerrainSections.emplace_back(index);
		m_TerrainSections.back().Generate(m_NoiseGenerator, m_Seed, m_MaxHeight, m_MapNoiseDistance);
	}

	void Terrain::Draw(sf::RenderWindow& render)
	{
		float sampleWidth = TERRAIN_SECTION_WIDTH / static_cast<float>(TERRAIN_SAMPLES_AMOUNT);
		for (const auto& section : m_TerrainSections)
		{
			m_DotMesh.setFillColor(sf::Color::White);
			float sectionStartPosition = static_cast<float>(section.Index) * TERRAIN_SECTION_WIDTH;
			for (size_t i = 0; i < section.Samples.size() - 1; ++i)
			{
				sf::Vector2f p1{ sectionStartPosition + i * sampleWidth, section.Samples.at(i)};
				sf::Vector2f p2{ sectionStartPosition + (i + 1) * sampleWidth, section.Samples.at(i + 1) };

				if (i == 0)
				{
					m_DotMesh.setFillColor(sf::Color::Yellow);
				}
				else
				{
					m_DotMesh.setFillColor(sf::Color::White);
				}

				m_DotMesh.setPosition(p1);
				render.draw(m_DotMesh);

				if (i == section.Samples.size() - 2)
				{
					m_DotMesh.setFillColor(sf::Color::Yellow);
				}
				else
				{
					m_DotMesh.setFillColor(sf::Color::White);
				}
				m_DotMesh.setPosition(p2);
				render.draw(m_DotMesh);
			}
		}
	}

	void Terrain::SetDotScale(float scale)
	{
		m_DotMesh.setScale({ scale, scale });
	}

} // CW
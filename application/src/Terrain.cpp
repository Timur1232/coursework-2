#include "pch.h"
#include "Terrain.h"

#include "debug_utils/Log.h"

namespace CW {

	TerrainSection::TerrainSection(int globalPosition)
		: GlobalPosition(globalPosition)
	{
		Samples.fill(0);
	}

	void TerrainSection::Generate(NoiseGenerator& gen, int seed, float maxHeight)
	{
		float x = static_cast<float>(GlobalPosition * TERRAIN_SECTION_SIZE);
		float stepX = 10.0f / static_cast<float>(TERRAIN_SECTION_SIZE);

		for (auto& sample : Samples)
		{
			float normalizedNoiseValue = (gen->GenSingle2D(x, 0.0f, seed) + 1.0f) / 2.0f;
			sample = static_cast<int>(normalizedNoiseValue * maxHeight);
			x += stepX;
		}
	}

	Terrain::Terrain()
		: m_NoiseGenerator(FastNoise::New<FastNoise::Perlin>())
	{
	}

	void Terrain::Generate(int position)
	{
		auto found = std::find_if(m_TerrainSections.begin(), m_TerrainSections.end(),
			[position](const TerrainSection& section) {
				return section.GlobalPosition == position;
			});
		if (found != m_TerrainSections.end())
		{
			CW_WARN("Trying to generate existing terrain section on position: {}", position);
			return;
		}

		m_TerrainSections.emplace_back(position);
		m_TerrainSections.back().Generate(m_NoiseGenerator, m_Seed, m_MaxHeight);
	}

	void Terrain::Draw(sf::RenderWindow& render)
	{
		float sampleWidth = 100.0f;
		for (const auto& section : m_TerrainSections)
		{
			for (size_t i = 0; i < section.Samples.size() - 1; ++i)
			{
				sf::Vector2f p1{ (float)(section.GlobalPosition * TERRAIN_SECTION_SIZE + i) * sampleWidth, (float)(section.Samples.at(i))};
				sf::Vector2f p2{ (float)(section.GlobalPosition * TERRAIN_SECTION_SIZE + i + 1) * sampleWidth, (float)(section.Samples.at(i + 1)) };

				m_LineMesh.SetPoint2(p2);
				m_LineMesh.SetPoint1(p1);
				render.draw(m_LineMesh);
			}
		}
	}

	void Terrain::SetLineThickness(float thickness) const
	{
		m_LineMesh.SetThickness(thickness);
	}

} // CW
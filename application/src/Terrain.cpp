#include "pch.h"
#include "Terrain.h"

#include "debug_utils/Log.h"
#include "utils/utils.h"

namespace CW {

	TerrainSection::TerrainSection(int index, size_t samplesCount)
		: Index(index), Samples(samplesCount)
	{
	}

	void TerrainSection::Generate(const NoiseGenerator& gen, float maxHeight, float mapNoiseDistance)
	{
		float noiseX = static_cast<float>(Index) * mapNoiseDistance;
		float noiseStep = mapNoiseDistance / static_cast<float>(Samples.size());

		for (auto& sample : Samples)
		{
			float noiseValue = gen.GenSingle2D(noiseX, 0.0f);
			sample = noiseValue * maxHeight;
			noiseX += noiseStep;
		}
	}


	Terrain::Terrain()
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

		m_TerrainSections.emplace_back(index, m_SamplesPerSection);
		m_TerrainSections.back().Generate(m_NoiseGenerator, m_MaxHeight, m_MapedNoiseDistance);
	}

	void Terrain::Draw(sf::RenderWindow& render)
	{
		float sampleWidth = m_SectionWidth / static_cast<float>(m_SamplesPerSection);
		for (const auto& section : m_TerrainSections)
		{
			float sectionStartPosition = static_cast<float>(section.Index) * m_SectionWidth;
			for (size_t i = 0; i < section.Samples.size() - 1; ++i)
			{
				sf::Vector2f p1{ sampleToWorldPosition(section, i, sectionStartPosition, sampleWidth) };
				sf::Vector2f p2{ sampleToWorldPosition(section, i + 1, sectionStartPosition, sampleWidth) };

				m_DotMesh.setPosition(p1);
				render.draw(m_DotMesh);
				m_DotMesh.setPosition(p2);
				render.draw(m_DotMesh);
			}
		}
	}

	void Terrain::RegenerateExisting()
	{
		for (auto& section : m_TerrainSections)
		{
			section.Samples.resize(m_SamplesPerSection);
			section.Generate(m_NoiseGenerator, m_MaxHeight, m_MapedNoiseDistance);
		}
	}

	bool Terrain::Near(const Object& object, float distThreashold) const
	{
		int sectionIndex = object.GetPos().x / m_SectionWidth;
		if (object.GetPos().x < 0)
			sectionIndex -= 1;

		int sampleIndex = (object.GetPos().x - sectionIndex * m_SectionWidth) / m_SamplesPerSection;

		auto section = std::ranges::find_if(m_TerrainSections.begin(), m_TerrainSections.end(),
			[sectionIndex](const TerrainSection& s) { return s.Index == sectionIndex; });

		if (section == m_TerrainSections.end())
		{
			return false;
		}

		float sampleWidth = m_SectionWidth / static_cast<float>(m_SamplesPerSection);
		float sectionStartPosition = static_cast<float>(section->Index) * m_SectionWidth;

		for (int i = -2; i <= 2; ++i)
		{
			if (sampleIndex + i < 0 || sampleIndex + i >= m_SamplesPerSection)
				continue;
			sf::Vector2f samplePos = sampleToWorldPosition(*section, sampleIndex + i, sectionStartPosition, sampleWidth);
			if (distance_squared(samplePos, object.GetPos()) <= distThreashold * distThreashold)
			{
				return true;
			}
		}

		return false;
	}

	void Terrain::SetDotScale(float scale)
	{
		m_DotMesh.setScale({ scale, scale });
	}

	sf::Vector2f Terrain::sampleToWorldPosition(const TerrainSection& section, size_t sampleIndex, float sectionStartPosition, float sampleWidth) const
	{
		sf::Vector2f pos{ sectionStartPosition + sampleIndex * sampleWidth, section.Samples.at(sampleIndex) - m_YOffset };
		return pos;
	}


	NoiseGenerator::NoiseGenerator()
		: m_NoiseTree(FastNoise::New<FastNoise::FractalFBm>())
	{
		m_NoiseTree->SetSource(FastNoise::New<FastNoise::Perlin>());
	}

} // CW
#include "pch.h"
#include "Terrain.h"

#include "debug_utils/Log.h"
#include "utils/utils.h"

namespace CW {

	TerrainSection::TerrainSection(int index, size_t samplesCount)
		: Key(index), Samples(samplesCount)
	{
	}

	void TerrainSection::Generate(const NoiseGenerator& gen, float maxHeight, float mapNoiseDistance)
	{
		float noiseX = static_cast<float>(Key) * mapNoiseDistance;
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

	void Terrain::Generate(int keyPosition)
	{
		if (GetSection(keyPosition) != m_TerrainSections.end())
		{
			CW_WARN("Trying to generate existing terrain section on position: {}", keyPosition);
			return;
		}

		m_TerrainSections.emplace_back(keyPosition, m_SamplesPerSection);
		m_TerrainSections.back().Generate(m_NoiseGenerator, m_MaxHeight, m_MapedNoiseDistance);
	}

	void Terrain::Draw(sf::RenderWindow& render)
	{
		float sampleWidth = calcSampleWidth();
		for (const auto& section : m_TerrainSections)
		{
			float sectionStartPosition = calcSectionStartPosition(section);
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

	std::vector<TerrainSection>::iterator Terrain::GetSection(int keyPosition)
	{
		return std::ranges::find_if(m_TerrainSections.begin(), m_TerrainSections.end(),
			[keyPosition](const TerrainSection& section) { return section.Key == keyPosition; });
	}

	std::vector<TerrainSection>::const_iterator Terrain::GetSection(int keyPosition) const
	{
		return std::ranges::find_if(m_TerrainSections.begin(), m_TerrainSections.end(),
			[keyPosition](const TerrainSection& section) { return section.Key == keyPosition; });
	}

	void Terrain::GenerateMesh(sf::ConvexShape& mesh, int keyPosition) const
	{
		auto section = GetSection(keyPosition);
		if (section == m_TerrainSections.end())
		{
			CW_ERROR("Section with key position {} don\'t exist!", keyPosition);
			return;
		}
		auto nextSection = GetSection(keyPosition + 1);

		if (nextSection != m_TerrainSections.end())
		{
			mesh.setPointCount(m_SamplesPerSection + 3);
		}
		else
		{
			mesh.setPointCount(m_SamplesPerSection + 2);
		}

		float sectionPosition = calcSectionStartPosition(*section);
		float sampleWidth = calcSampleWidth();

		size_t pointIndex = 0;
		while (pointIndex < section->Samples.size())
		{
			mesh.setPoint(pointIndex, sampleToWorldPosition(*section, pointIndex, sectionPosition, sampleWidth));
			pointIndex++;
		}

		sf::Vector2f bottomPoint = sampleToWorldPosition(*section, section->Samples.size() - 1, sectionPosition, sampleWidth);
		if (nextSection != m_TerrainSections.end())
		{
			sf::Vector2f nextPointPos = sampleToWorldPosition(*nextSection, 0,
				calcSectionStartPosition(*nextSection), sampleWidth);
			mesh.setPoint(pointIndex, nextPointPos);
			pointIndex++;
			bottomPoint = nextPointPos;
		}

		float bottomPointHeight = 100000.0f;

		bottomPoint.y = bottomPointHeight;
		mesh.setPoint(pointIndex, bottomPoint);
		pointIndex++;

		bottomPoint = sampleToWorldPosition(*section, 0, sectionPosition, sampleWidth);
		bottomPoint.y = bottomPointHeight;
		mesh.setPoint(pointIndex, bottomPoint);
	}

	void Terrain::GenerateAllMeshes(std::vector<sf::ConvexShape>& meshes) const
	{
		meshes.resize(m_TerrainSections.size());
		for (size_t i = 0; i < m_TerrainSections.size(); i++)
		{
			GenerateMesh(meshes[i], m_TerrainSections[i].Key);
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

	bool Terrain::IsNear(const Object& object, float distThreashold, int range) const
	{
		int sectionKey = calcSectionKeyPosition(object.GetPos().x);
		float sampleWidth = calcSampleWidth();
		int sampleIndex = calcSignedSampleIndex(object.GetPos().x, sectionKey, sampleWidth);

		auto section = GetSection(sectionKey);
		if (section == m_TerrainSections.end())
		{
			return false;
		}

		// TODO: определение коллизии на границах секций
		/*auto rightSection = section + 1;
		auto leftSection = m_TerrainSections.end();
		if (section != m_TerrainSections.begin())
		{
			leftSection = section - 1;
		}*/

		float sectionStartPosition = calcSectionStartPosition(*section);

		for (int i = -range; i <= range; ++i)
		{
			/*if (sampleIndex + i < 0 && leftSection != m_TerrainSections.end())
			{
				samplePos = sampleToWorldPosition(*leftSection, m_SamplesPerSection - 1 + i, sectionStartPosition - m_SectionWidth, sampleWidth);
			}
			else if (sampleIndex + i >= m_SamplesPerSection && rightSection != m_TerrainSections.end())
			{
				samplePos = sampleToWorldPosition(*leftSection, i, sectionStartPosition + m_SectionWidth, sampleWidth);
			}*/
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

	float Terrain::calcSectionStartPosition(const TerrainSection& section) const
	{
		return static_cast<float>(section.Key) * m_SectionWidth;
	}

	float Terrain::calcSampleWidth() const
	{
		return m_SectionWidth / static_cast<float>(m_SamplesPerSection);
	}

	int Terrain::calcSectionKeyPosition(float xPos) const
	{
		int sectionKey = static_cast<int>(xPos / m_SectionWidth);
		if (xPos < 0)
			sectionKey -= 1;
		return sectionKey;
	}

	int CW::Terrain::calcSignedSampleIndex(float xPos, int sectionKeyPosition, float sampleWidth) const
	{
		return static_cast<int>((xPos - sectionKeyPosition * m_SectionWidth) / sampleWidth);
	}


	NoiseGenerator::NoiseGenerator()
		: m_NoiseTree(FastNoise::New<FastNoise::FractalFBm>())
	{
		m_NoiseTree->SetSource(FastNoise::New<FastNoise::Perlin>());
	}

} // CW
#include "pch.h"
#include "Terrain.h"

#include "debug_utils/Log.h"
#include "utils/utils.h"
#include "engine/Renderer.h"

namespace CW {

	float NoiseGenerator::GenSingle2D(float x, float y) const
	{
		return m_NoiseTreeBase->GenSingle2D(x * m_BaseFrequensy, y * m_BaseFrequensy, m_BaseSeed) * m_BaseFactor
			+ m_NoiseTreeDetailed->GenSingle2D(x, y, m_DetailedSeed) * m_DetailedFactor;
	}


	float deepness_func(float x, float bellWidth, float maxHeight)
	{
		float exponent = -(x * x) / (2.0f * (bellWidth * bellWidth));
		return maxHeight - maxHeight * std::exp(exponent);
	}

	TerrainSection::TerrainSection(int index, size_t samplesCount)
		: Key(index), Samples(samplesCount)
	{
	}

	void TerrainSection::Generate(const NoiseGenerator& gen, float maxHeight, float mapNoiseDistance, float sectionWidth, float bellWidth, float bellHeight)
	{
		float realX = static_cast<float>(Key) * sectionWidth;
		float realStep = sectionWidth / static_cast<float>(Samples.size() - 1);
		float noiseX = static_cast<float>(Key) * mapNoiseDistance;
		float noiseStep = mapNoiseDistance / static_cast<float>(Samples.size() - 1);

		for (auto& sample : Samples)
		{
			float noiseValue = gen.GenSingle2D(noiseX, 0.0f);
			sample = noiseValue * maxHeight + deepness_func(realX, bellWidth, bellHeight);
			noiseX += noiseStep;
			realX += realStep;
		}
	}


	Terrain::Terrain()
	{
		m_DotMesh.setOrigin(m_DotMesh.getGeometricCenter());
	}

	bool CW::Terrain::Generate(int keyPosition)
	{
		if (GetSection(keyPosition) != m_TerrainSections.end())
		{
			CW_WARN("Trying to generate existing terrain section on position: {}", keyPosition);
			return false;
		}

		m_TerrainSections.emplace_back(keyPosition, m_SamplesPerSection + 1);
		m_TerrainSections.back().Generate(m_NoiseGenerator, m_MaxHeight, m_MapedNoiseDistance, m_SectionWidth, m_BellWidth, m_BellHeigth);
		return true;
	}

	void Terrain::DebugDraw()
	{
		float sampleWidth = calcSampleWidth();
		for (const auto& section : m_TerrainSections)
		{
			float sectionStartPosition = CalcSectionStartPosition(section.Key);
			for (size_t i = 0; i < m_SamplesPerSection - 1; ++i)
			{
				sf::Vector2f p1{ sampleToWorldPosition(section, i, sectionStartPosition, sampleWidth) };
				sf::Vector2f p2{ sampleToWorldPosition(section, i + 1, sectionStartPosition, sampleWidth) };

				Renderer::Get().BeginDotShape()
					.Position(p1)
					.Draw()
					.Position(p2)
					.Draw()
					.SetDefault();
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
		//
		mesh.setPointCount(m_SamplesPerSection + 3);

		float sectionPosition = CalcSectionStartPosition(section->Key);
		float sampleWidth = calcSampleWidth();

		size_t pointIndex = 0;
		while (pointIndex < section->Samples.size())
		{
			mesh.setPoint(pointIndex, sampleToWorldPosition(*section, pointIndex, sectionPosition, sampleWidth));
			pointIndex++;
		}

		sf::Vector2f bottomPoint = sampleToWorldPosition(*section, section->Samples.size() - 1, sectionPosition, sampleWidth);

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
			section.Samples.resize(m_SamplesPerSection + 1);
			section.Generate(m_NoiseGenerator, m_MaxHeight, m_MapedNoiseDistance, m_SectionWidth, m_BellWidth, m_BellHeigth);
		}
	}

	bool Terrain::IsNear(const Object& object, float distThreashold, int range) const
	{
		int sectionKey = CalcSectionKeyPosition(object.GetPos().x);
		float sampleWidth = calcSampleWidth();
		int sampleIndex = calcSignedSampleIndex(object.GetPos().x, sectionKey, sampleWidth);

		auto section = GetSection(sectionKey);
		if (section == m_TerrainSections.end())
		{
			return false;
		}

		// TODO: определение коллизии на границах секций

		float sectionStartPosition = CalcSectionStartPosition(section->Key);

		for (int i = -range; i <= range; ++i)
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

	float Terrain::CalcSectionStartPosition(int key) const
	{
		return static_cast<float>(key) * m_SectionWidth;
	}

	float Terrain::calcSampleWidth() const
	{
		return m_SectionWidth / static_cast<float>(m_SamplesPerSection);
	}

	int Terrain::CalcSectionKeyPosition(float xPos) const
	{
		int sectionKey = static_cast<int>(xPos / m_SectionWidth);
		if (xPos < 0)
			sectionKey -= 1;
		return sectionKey;
	}

	int Terrain::calcSignedSampleIndex(float xPos, int sectionKeyPosition, float sampleWidth) const
	{
		return static_cast<int>((xPos - sectionKeyPosition * m_SectionWidth) / sampleWidth);
	}


	NoiseGenerator::NoiseGenerator()
		: m_NoiseTreeDetailed(FastNoise::New<FastNoise::FractalFBm>()),
		  m_NoiseTreeBase(FastNoise::New<FastNoise::FractalFBm>())
	{
		m_NoiseTreeDetailed->SetSource(FastNoise::New<FastNoise::Perlin>());
		m_NoiseTreeBase->SetSource(FastNoise::New<FastNoise::Perlin>());
	}


} // CW
#include "pch.h"
#include "Terrain.h"

#include "debug_utils/Log.h"
#include "utils/utils.h"
#include "engine/Renderer.h"

namespace CW {

	//==================================[NoiseGenerator]===================================//

	float NoiseGenerator::GenSingle2D(float x, float y) const
	{
		return m_NoiseTreeBase->GenSingle2D(x * m_BaseFrequensy, y * m_BaseFrequensy, m_BaseSeed) * m_BaseFactor
			+ m_NoiseTreeDetailed->GenSingle2D(x * m_DetailedFrequensy, y * m_DetailedFrequensy, m_DetailedSeed) * m_DetailedFactor;
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

	//==================================[Terrain]===================================//

	bool Terrain::Generate(int keyPosition, const NoiseGenerator& gen,
		float maxHeight, float mapNoiseDistance, float bellWidth, float bellHeight)
	{
		if (GetSection(keyPosition) != TerrainSections.end())
		{
			CW_WARN("Trying to generate existing terrain section on position: {}", keyPosition);
			return false;
		}

		TerrainSections.emplace_back(keyPosition, SamplesPerSection + 1);
		TerrainSections.back().Generate(gen, maxHeight, mapNoiseDistance, SectionWidth, bellWidth, bellHeight);
		return true;
	}

	void Terrain::RegenerateExisting(const NoiseGenerator& gen,
		float maxHeight, float mapNoiseDistance, float bellWidth, float bellHeight)
	{
		for (auto& section : TerrainSections)
		{
			section.Samples.resize(SamplesPerSection + 1);
			section.Generate(gen, maxHeight, mapNoiseDistance, SectionWidth, bellWidth, bellHeight);
		}
	}

	std::vector<TerrainSection>::iterator Terrain::GetSection(int keyPosition)
	{
		return std::ranges::find_if(TerrainSections.begin(), TerrainSections.end(),
			[keyPosition](const TerrainSection& section) { return section.Key == keyPosition; });
	}

	std::vector<TerrainSection>::const_iterator Terrain::GetSection(int keyPosition) const
	{
		return std::ranges::find_if(TerrainSections.begin(), TerrainSections.end(),
			[keyPosition](const TerrainSection& section) { return section.Key == keyPosition; });
	}

	float Terrain::GetHeight(float x) const
	{
		int key = CalcSectionKeyPosition(x);
		auto section = GetSection(key);
		size_t sample = static_cast<size_t>(CalcSignedSampleIndex(x, key, CalcSampleWidth()));
		if (section == TerrainSections.end())
		{
			CW_ERROR("Terrain section on position x: {}, not exist!", x);
			return 0.0f;
		}
		return SampleToWorldPosition(*section, sample, CalcSectionStartPosition(key), CalcSampleWidth()).y;
	}

	sf::Vector2f Terrain::GetNormal(float x) const
	{
		float sampleWidth = CalcSampleWidth();

		int key = CalcSectionKeyPosition(x);
		auto section = GetSection(key);
		if (section == TerrainSections.end())
		{
			CW_ERROR("Terrain section1 on position x: {}, not exist!", x);
			return {1.0f, 0.0f};
		}

		size_t sample1 = static_cast<size_t>(CalcSignedSampleIndex(x, key, sampleWidth));
		size_t sample2 = static_cast<size_t>(CalcSignedSampleIndex(x + sampleWidth, key, sampleWidth));

		sf::Vector2f p1 = SampleToWorldPosition(*section, sample1, CalcSectionStartPosition(key), sampleWidth);
		sf::Vector2f p2 = SampleToWorldPosition(*section, sample2, CalcSectionStartPosition(key), sampleWidth);

		sf::Vector2f normal = (p1 - p2).perpendicular().normalized();
		return normal;
	}

	bool Terrain::IsNear(const Object& object, float distThreashold, int range) const
	{
		int sectionKey = CalcSectionKeyPosition(object.GetPos().x);
		float sampleWidth = CalcSampleWidth();
		int sampleIndex = CalcSignedSampleIndex(object.GetPos().x, sectionKey, sampleWidth);

		auto section = GetSection(sectionKey);
		if (section == TerrainSections.end())
		{
			return false;
		}

		// TODO: определение коллизии на границах секций

		float sectionStartPosition = CalcSectionStartPosition(section->Key);

		for (int i = -range; i <= range; ++i)
		{
			if (sampleIndex + i < 0 || sampleIndex + i >= SamplesPerSection)
				continue;

			sf::Vector2f samplePos = SampleToWorldPosition(*section, sampleIndex + i, sectionStartPosition, sampleWidth);
			if (distance_squared(samplePos, object.GetPos()) <= distThreashold * distThreashold)
			{
				return true;
			}
		}

		return false;
	}

	float Terrain::CalcSectionStartPosition(int key) const
	{
		return static_cast<float>(key) * SectionWidth;
	}

	int Terrain::CalcSectionKeyPosition(float xPos) const
	{
		int sectionKey = static_cast<int>(xPos / SectionWidth);
		if (xPos < 0)
			sectionKey -= 1;
		return sectionKey;
	}

	sf::Vector2f Terrain::SampleToWorldPosition(const TerrainSection& section, size_t sampleIndex, float sectionStartPosition, float sampleWidth) const
	{
		sf::Vector2f pos{ sectionStartPosition + sampleIndex * sampleWidth, section.Samples.at(sampleIndex) - YOffset };
		return pos;
	}

	float Terrain::CalcSampleWidth() const
	{
		return SectionWidth / static_cast<float>(SamplesPerSection);

	}

	int Terrain::CalcSignedSampleIndex(float xPos, int sectionKeyPosition, float sampleWidth) const
	{
		return static_cast<int>((xPos - sectionKeyPosition * SectionWidth) / sampleWidth);
	}

	//==================================[TerrainGenerator]===================================//

	TerrainGenerator::TerrainGenerator(const TerrainGenerationSettings& settings)
	{
		SetSettings(settings);
	}

	void TerrainGenerator::SetSettings(const TerrainGenerationSettings& settings)
	{
		m_MaxHeight = settings.MaxHeight;
		m_BellHeight = settings.BellHeigth;
		m_BellWidth = settings.BellWidth;
		m_MapedNoiseDistance = settings.MappedNoiseDistance;
		m_Terrain.SamplesPerSection = settings.SamplesPerSection;
		m_Terrain.SectionWidth = settings.SectionWidth;
		m_Terrain.YOffset = settings.YOffset;

		m_NoiseGenerator.SetBaseFrequensy(settings.BaseFrequensy);
		m_NoiseGenerator.SetDetailedFrequensy(settings.DetailedFrequensy);
		m_NoiseGenerator.SetBaseFactor(settings.BaseFactor);
		m_NoiseGenerator.SetDetailedFactor(settings.DetailedFactor);

		m_NoiseGenerator.SetBaseSeed(settings.BaseSeed);
		m_NoiseGenerator.SetDetailedSeed(settings.DetailedSeed);

		m_NoiseGenerator.SetGain(settings.Gain);
		m_NoiseGenerator.SetWeightedStrength(settings.WeightedStrength);
		m_NoiseGenerator.SetOctaves(settings.Octaves);
		m_NoiseGenerator.SetLacunarity(settings.Lacunarity);
	}

	bool TerrainGenerator::Generate(int keyPosition)
	{
		return m_Terrain.Generate(keyPosition, m_NoiseGenerator, m_MaxHeight, m_MapedNoiseDistance, m_BellWidth, m_BellHeight);
	}

	void TerrainGenerator::DebugDraw()
	{
		float sampleWidth = CalcSampleWidth();
		for (const auto& section : m_Terrain.TerrainSections)
		{
			float sectionStartPosition = CalcSectionStartPosition(section.Key);
			for (size_t i = 0; i < m_Terrain.SamplesPerSection - 1; ++i)
			{
				sf::Vector2f p1{ SampleToWorldPosition(section, i, sectionStartPosition, sampleWidth) };
				sf::Vector2f p2{ SampleToWorldPosition(section, i + 1, sectionStartPosition, sampleWidth) };

				Renderer::Get().BeginDotShape()
					.Position(p1)
					.Draw()
					.Position(p2)
					.Draw()
					.SetDefault();
			}
		}
	}

	std::vector<TerrainSection>::iterator TerrainGenerator::GetSection(int keyPosition)
	{
		return m_Terrain.GetSection(keyPosition);
	}

	std::vector<TerrainSection>::const_iterator TerrainGenerator::GetSection(int keyPosition) const
	{
		return m_Terrain.GetSection(keyPosition);
	}

	float TerrainGenerator::GetHeight(float x) const
	{
		return m_Terrain.GetHeight(x);
	}

	sf::Vector2f TerrainGenerator::GetNormal(float x) const
	{
		return m_Terrain.GetNormal(x);
	}

	void TerrainGenerator::RegenerateExisting()
	{
		m_Terrain.RegenerateExisting(m_NoiseGenerator, m_MaxHeight, m_MapedNoiseDistance, m_BellWidth, m_BellHeight);
	}

	bool TerrainGenerator::IsNear(const Object& object, float distThreashold, int range) const
	{
		return m_Terrain.IsNear(object, distThreashold, range);
	}

	sf::Vector2f TerrainGenerator::SampleToWorldPosition(const TerrainSection& section, size_t sampleIndex, float sectionStartPosition, float sampleWidth) const
	{
		return m_Terrain.SampleToWorldPosition(section, sampleIndex, sectionStartPosition, sampleWidth);
	}

	float TerrainGenerator::CalcSectionStartPosition(int key) const
	{
		return m_Terrain.CalcSectionStartPosition(key);
	}

	float TerrainGenerator::CalcSampleWidth() const
	{
		return m_Terrain.CalcSampleWidth();
	}

	int TerrainGenerator::CalcSectionKeyPosition(float xPos) const
	{
		return m_Terrain.CalcSectionKeyPosition(xPos);
	}

	int TerrainGenerator::CalcSignedSampleIndex(float xPos, int sectionKeyPosition, float sampleWidth) const
	{
		return m_Terrain.CalcSignedSampleIndex(xPos, sectionKeyPosition, sampleWidth);
	}


	NoiseGenerator::NoiseGenerator()
		: m_NoiseTreeDetailed(FastNoise::New<FastNoise::FractalFBm>()),
		  m_NoiseTreeBase(FastNoise::New<FastNoise::FractalFBm>())
	{
		m_NoiseTreeDetailed->SetSource(FastNoise::New<FastNoise::Perlin>());
		m_NoiseTreeBase->SetSource(FastNoise::New<FastNoise::Perlin>());
	}

	void generate_mesh(const Terrain& terrain, sf::ConvexShape& mesh, int keyPosition)
	{
		auto section = terrain.GetSection(keyPosition);
		if (section == terrain.TerrainSections.end())
		{
			CW_ERROR("Section with key position {} don\'t exist!", keyPosition);
			return;
		}
		mesh.setPointCount(terrain.SamplesPerSection + 3);

		float sectionPosition = terrain.CalcSectionStartPosition(section->Key);
		float sampleWidth = terrain.CalcSampleWidth();

		size_t pointIndex = 0;
		while (pointIndex < section->Samples.size())
		{
			mesh.setPoint(pointIndex, terrain.SampleToWorldPosition(*section, pointIndex, sectionPosition, sampleWidth));
			pointIndex++;
		}

		sf::Vector2f bottomPoint = terrain.SampleToWorldPosition(*section, section->Samples.size() - 1, sectionPosition, sampleWidth);

		float bottomPointHeight = 100000.0f;

		bottomPoint.y = bottomPointHeight;
		mesh.setPoint(pointIndex, bottomPoint);
		pointIndex++;

		bottomPoint = terrain.SampleToWorldPosition(*section, 0, sectionPosition, sampleWidth);
		bottomPoint.y = bottomPointHeight;
		mesh.setPoint(pointIndex, bottomPoint);
	}

	void generate_all_meshes(const Terrain& terrain, std::vector<sf::ConvexShape>& meshes)
	{
		meshes.resize(terrain.TerrainSections.size());
		for (size_t i = 0; i < terrain.TerrainSections.size(); i++)
		{
			generate_mesh(terrain, meshes[i], terrain.TerrainSections[i].Key);
		}
	}

} // CW
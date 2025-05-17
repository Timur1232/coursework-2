#pragma once

#include "pch.h"

#include "engine/IDrawable.h"
#include "debug_utils/LineShape.h"
#include "Object.h"

namespace CW {

	class NoiseGenerator
	{
	public:
		NoiseGenerator();

		float GenSingle2D(float x, float y) const { return m_NoiseTree->GenSingle2D(x, y, m_Seed); }

		void SetSeed(int seed) { m_Seed = seed; }
		int GetSeed() const { return m_Seed; }

		void SetGain(float gain) { m_Gain = gain; m_NoiseTree->SetGain(gain); }
		void SetWeightedStrength(float strength) { m_WeightedStrength = strength; m_NoiseTree->SetWeightedStrength(strength); }
		void SetOctaves(int octaves) { m_Octaves = octaves; m_NoiseTree->SetOctaveCount(octaves); }
		void SetLacunarity(float lacunarity) { m_Lacunarity = lacunarity; m_NoiseTree->SetLacunarity(lacunarity); }

		float GetGain() const { return m_Gain; }
		float GetWeightedStrength() const { return m_WeightedStrength; }
		int GetOctaves() const { return m_Octaves; }
		float GetLacunarity() const { return m_Lacunarity; }

	private:
		FastNoise::SmartNode<FastNoise::FractalFBm> m_NoiseTree;

		int m_Seed = 69420;
		float m_Gain = 0.5f;
		float m_WeightedStrength = 0.0f;
		int m_Octaves = 3;
		float m_Lacunarity = 2.0f;
	};


	struct TerrainSection
	{
		TerrainSection(int index, size_t samplesCount);

		void Generate(const NoiseGenerator& gen, float maxHeight, float sampleWidth);

		int Key;
		std::vector<float> Samples;
	};


	class Terrain
	{
	public:
		Terrain();

		void Generate(int keyPosition);
		void DebugDraw(sf::RenderWindow& render);

		std::vector<TerrainSection>::iterator GetSection(int keyPosition);
		std::vector<TerrainSection>::const_iterator GetSection(int keyPosition) const;

		void GenerateMesh(sf::ConvexShape& mesh, int keyPosition) const;
		void GenerateAllMeshes(std::vector<sf::ConvexShape>& meshes) const;

		void SetYOffset(float offset) { m_YOffset = offset; }
		float GetYOffset() const { return m_YOffset; }

		void SetMaxHeight(float value) { m_MaxHeight = value; }
		void SetMapedNoiseDistance(float value) { m_MapedNoiseDistance = value; }
		void SetSeed(int seed) { m_NoiseGenerator.SetSeed(seed); }
		void SetSamplesPerSection(size_t samplesCount) { m_SamplesPerSection = samplesCount; }
		void SetSectionWidth(float sectionWidth) { m_SectionWidth = sectionWidth; }

		void SetGain(float gain) { m_NoiseGenerator.SetGain(gain); }
		void SetWeightedStrength(float strength) { m_NoiseGenerator.SetWeightedStrength(strength); }
		void SetOctaves(int octaves) { m_NoiseGenerator.SetOctaves(octaves); }
		void SetLacunarity(float lacunarity) { m_NoiseGenerator.SetLacunarity(lacunarity); }

		int GetSeed() const { return m_NoiseGenerator.GetSeed(); }
		float GetMaxHeight() const { return m_MaxHeight; }
		float GetMappedNoiseDistance() const { return m_MapedNoiseDistance; }
		size_t GetSamplesPerSection() const { return m_SamplesPerSection; }
		float GetSectionWidth() const { return m_SectionWidth; }

		float GetGain() const { return m_NoiseGenerator.GetGain(); }
		float GetWeightedStrength() const { return m_NoiseGenerator.GetWeightedStrength(); }
		int GetOctaves() const { return m_NoiseGenerator.GetOctaves(); }
		float GetLacunarity() const { return m_NoiseGenerator.GetLacunarity(); }

		void RegenerateExisting();

		[[nodiscard]] bool IsNear(const Object& object, float distThreashold, int range = 3) const;

		// Debug
		void SetDotScale(float size);

	private:
		[[nodiscard]] sf::Vector2f sampleToWorldPosition(
			const TerrainSection& section, size_t sampleIndex,
			float sectionStartPosition, float sampleWidth) const;

		[[nodiscard]] float calcSectionStartPosition(const TerrainSection& section) const;
		[[nodiscard]] float calcSampleWidth() const;
		[[nodiscard]] int calcSectionKeyPosition(float xPos) const;
		int calcSignedSampleIndex(float xPos, int sectionKeyPosition, float sampleWidth) const;

	private:
		std::vector<TerrainSection> m_TerrainSections;

		NoiseGenerator m_NoiseGenerator;

		float m_MaxHeight = 1000.0f;
		float m_MapedNoiseDistance = 0.5f;

		size_t m_SamplesPerSection = 15;
		float m_SectionWidth = 1000.0f;
		float m_YOffset = -3000.0f;

		// Debug
		sf::CircleShape m_DotMesh{3.0f, 4};
	};

} // CW
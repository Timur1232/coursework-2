#pragma once

#include "pch.h"

#include "engine/IDrawable.h"
#include "debug_utils/LineShape.h"

namespace CW {

	constexpr size_t TERRAIN_SAMPLES_AMOUNT = 100;
	constexpr float TERRAIN_SECTION_WIDTH = 1000.0f;

	using NoiseGenerator = FastNoise::SmartNode<FastNoise::Perlin>;

	struct TerrainSection
	{
		TerrainSection(int globalPosition);

		void Generate(NoiseGenerator& gen, int seed, float maxHeight, float sampleWidth);

		int Index;
		std::array<float, TERRAIN_SAMPLES_AMOUNT> Samples;
	};

	class Terrain
		: public IDrawable
	{
	public:
		Terrain();

		void Generate(int position);
		void Draw(sf::RenderWindow& render) override;

		// Debug
		void SetDotScale(float size);

	private:
		std::vector<TerrainSection> m_TerrainSections;
		NoiseGenerator m_NoiseGenerator;
		int m_Seed = 69420;

		float m_MaxHeight = 100.0f;
		float m_MapNoiseDistance = 5.0f;

		// Debug
		mutable sf::CircleShape m_DotMesh{3.0f, 4};
	};

} // CW
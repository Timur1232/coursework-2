#pragma once

#include "pch.h"

#include "engine/IDrawable.h"
#include "debug_utils/LineShape.h"

namespace CW {

	constexpr size_t TERRAIN_SECTION_SIZE = 150;
	using NoiseGenerator = FastNoise::SmartNode<FastNoise::Perlin>;

	struct TerrainSection
	{
		TerrainSection(int globalPosition);
		/*TerrainSection(TerrainSection&& other);
		const TerrainSection& operator=(TerrainSection&& other);*/

		void Generate(NoiseGenerator& gen, int seed, float maxHeight);

		int GlobalPosition;
		std::array<int, TERRAIN_SECTION_SIZE> Samples;
	};

	class Terrain
		: public IDrawable
	{
	public:
		Terrain();

		void Generate(int position);

		void Draw(sf::RenderWindow& render) const override;

		// Debug
		void SetLineThickness(float thickness) const;

	private:
		std::vector<TerrainSection> m_TerrainSections;
		NoiseGenerator m_NoiseGenerator;
		int m_Seed = 69420;

		float m_MaxHeight = 1000.0f;

		// Debug
		mutable LineShape m_LineMesh;
	};

} // CW
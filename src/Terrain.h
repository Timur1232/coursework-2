#pragma once

#include "pch.h"

#include "engine/IDrawable.h"
#include "debug_utils/LineShape.h"

namespace CW {

	constexpr size_t TERRAIN_SECTION_SIZE = 50;

	struct TerrainSection
	{
		TerrainSection(int globalPosition);
		TerrainSection(TerrainSection&& other);
		const TerrainSection& operator=(TerrainSection&& other);

		void Generate(FastNoise& gen, float maxHeight, float stepX = 1.0f);

		int GlobalPosition;
		std::vector<int> Samples;
	};

	class Terrain
		: public IDrawable
	{
	public:
		Terrain() = default;

		void Generate(int position);

		void Draw(sf::RenderWindow& render) const override;

	private:
		std::vector<TerrainSection> m_TerrainSections;
		FastNoise m_NoiseGenerator;

		float m_MaxHeight = 100.0f;
		float m_StepX = 1.0f;

		// Debug
		mutable LineShape m_LineMesh;
	};

} // CW
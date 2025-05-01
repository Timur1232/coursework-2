#include "pch.h"
#include "Terrain.h"

#include "debug_utils/Log.h"

namespace CW {

	TerrainSection::TerrainSection(int globalPosition)
		: GlobalPosition(globalPosition)
	{
		Samples.fill(0);
	}

	/*TerrainSection::TerrainSection(TerrainSection&& other)
		: GlobalPosition(other.GlobalPosition), Samples(std::forward<std::vector<int>>(other.Samples))
	{
	}

	const TerrainSection& TerrainSection::operator=(TerrainSection&& other)
	{
		GlobalPosition = other.GlobalPosition;
		Samples = std::forward<std::vector<int>>(other.Samples);
		return *this;
	}*/

	void TerrainSection::Generate(FastNoise& gen, float maxHeight, float stepX)
	{
		float x = static_cast<float>(GlobalPosition * TERRAIN_SECTION_SIZE);
		for (auto& sample : Samples)
		{
			float normalizedNoiseValue = (gen.GetPerlin(x, 0.0f) + 1.0f) / 2.0f;
			sample = normalizedNoiseValue * maxHeight;
			x += stepX;
		}
	}

	Terrain::Terrain()
	{
		m_NoiseGenerator.SetSeed(std::time(0));
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
		m_TerrainSections.back().Generate(m_NoiseGenerator, m_MaxHeight, m_StepX);
	}

	void Terrain::Draw(sf::RenderWindow& render) const
	{
		float sampleWidth = 100.0f;
		//ImGui::Begin("TerrainSection");
		for (const auto& section : m_TerrainSections)
		{
			for (size_t i = 0; i < section.Samples.size() - 1; ++i)
			{
				sf::Vector2f p1{ (float)(section.GlobalPosition * TERRAIN_SECTION_SIZE + i) * sampleWidth, (float)(section.Samples.at(i))};
				sf::Vector2f p2{ (float)(section.GlobalPosition * TERRAIN_SECTION_SIZE + i + 1) * sampleWidth, (float)(section.Samples.at(i + 1)) };

				m_LineMesh.SetPoint2(p2);
				m_LineMesh.SetPoint1(p1);
				render.draw(m_LineMesh);

				/*ImGui::Text("index: %d -> %d", i, i+1);
				ImGui::Text("p1: (%.2f, %.2f)", p1.x, p1.y);
				ImGui::Text("p2: (%.2f, %.2f)", p2.x, p2.y);
				ImGui::Spacing();*/
			}
		}
		//ImGui::End();
	}

	void Terrain::SetLineThickness(float thickness) const
	{
		m_LineMesh.SetThickness(thickness);
	}

} // CW
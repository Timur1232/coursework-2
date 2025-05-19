#pragma once

namespace CW {

	struct TerrainGenerationSettings
	{
		float MaxHeight;
		float BellHeigth;
		float BellWidth;

		int SamplesPerSection;
		float SectionWidth;
		float YOffset;

		float MappedNoiseDistance;
		float BaseFrequensy;
		float BaseFactor;
		float DetailedFactor;
		int DetailedSeed;
		int BaseSeed;
		float Gain;
		float WeightedStrength;
		int Octaves;
		float Lacunarity;

		TerrainGenerationSettings() { SetDefault(); }
		void SetDefault()
		{
			MaxHeight = 1000.0f;
			BellHeigth = 8000.0f;
			BellWidth = 15000.0f;

			SamplesPerSection = 15;
			SectionWidth = 1000.0f;
			YOffset = -3000.0f;

			MappedNoiseDistance = 0.5f;
			BaseFrequensy = 0.3f;
			BaseFactor = 2.6f;
			DetailedFactor = 0.8f;
			DetailedSeed = 69420;
			BaseSeed = 42069;
			Gain = 0.5f;
			WeightedStrength = 0.0f;
			Octaves = 3;
			Lacunarity = 2.0f;
		}
	};

} // CW
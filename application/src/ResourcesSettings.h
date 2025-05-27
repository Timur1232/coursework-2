#pragma once

namespace CW {

	struct ResourcesSettings
	{
		float GenerateChance;
		float ClusterSize;
		int MaxResourcesInCluster;
		int MaxResourceAmount;

		ResourcesSettings() { SetDefault(); }
		void SetDefault()
		{
			GenerateChance = 0.35f;
			ClusterSize = 350.0f;
			MaxResourcesInCluster = 15;
			MaxResourceAmount = 50;
		}
	};

} // CW
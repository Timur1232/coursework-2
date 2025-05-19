#pragma once

namespace CW {

	struct ResourcesSettings
	{
		float GenerateChance = 0.2f;
		float ClusterSize = 350.0f;
		int MaxResourcesInCluster = 20;
		int MaxResourceAmount = 50;

		ResourcesSettings() { SetDefault(); }
		void SetDefault()
		{
			GenerateChance = 0.2f;
			ClusterSize = 350.0f;
			MaxResourcesInCluster = 20;
			MaxResourceAmount = 50;
		}
	};

} // CW
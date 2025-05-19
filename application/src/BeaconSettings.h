#pragma once

namespace CW {

	struct BeaconSettings
	{
		float ChargeThreshold;
		float DischargeRate;

		BeaconSettings()
		{
			SetDefault();
		}
		void SetDefault()
		{
			ChargeThreshold = 0.05f;
			DischargeRate = 0.2f;
		}
	};

} // CW
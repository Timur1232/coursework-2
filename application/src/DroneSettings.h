#pragma once
#include "pch.h"

namespace CW {

	struct DroneSettings
	{
		float Speed;
		float TurningSpeed;

		float FOV;

		sf::Vector2f ViewDistance;
		float PickupDist;

		float BeaconCooldownSec;
		float WanderCooldownSec;
		float SpawnDroneCooldownSec;

		float RandomWanderAngleRad;
		float WanderAngleThresholdDeg;
		float MaxTurningDeltaRad;

		DroneSettings();
		void SetDefault();
	};

} // CW
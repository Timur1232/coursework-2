#pragma once
#include "pch.h"

namespace CW {

	struct DroneSettings
	{
		float Speed;
		sf::Angle TurningSpeed;

		float FOV;
		float FOVRad;

		sf::Vector2f ViewDistance;
		float PickupDist;

		float BeaconCooldownSec;
		float WanderCooldownSec;
		float SpawnDroneCooldownSec;

		sf::Angle RandomWanderAngle;
		sf::Angle WanderAngleThreshold;
		sf::Angle MaxTurningDelta;

		DroneSettings();
		void SetDefault();
	};

} // CW
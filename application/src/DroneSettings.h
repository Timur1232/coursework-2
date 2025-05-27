#pragma once
#include "pch.h"

namespace CW {

	struct DroneSettings
	{
		float DischargeRate;
		float Speed;
		float TurningSpeed;

		float FOV;

		sf::Vector2f ViewDistance;

		float BeaconCooldownSec;
		float WanderCooldownSec;
		float SpawnDroneCooldownSec;

		float RandomWanderAngleRad;
		float WanderAngleThresholdDeg;
		float MaxTurningDeltaRad;

		int DroneCost;
		float DroneSpawnCooldown;
		sf::Vector2f BasePosition;
		bool BitDirections;

		DroneSettings();
		void SetDefault();
	};

} // CW
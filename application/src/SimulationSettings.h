#pragma once
#include "pch.h"

#include "DroneSettings.h"
#include "BeaconSettings.h"
#include "ResourcesSettings.h"
#include "TerrainGenerationSettings.h"

namespace CW {

    struct SimulationSettings
    {
        DroneSettings Drones;
        BeaconSettings Beacons;
        ResourcesSettings Resources;
        TerrainGenerationSettings Terrain;

        int DronesCount;
        float StartingHorizontalPosition;

        SimulationSettings() { SetDefault(); }
        void SetDefault()
        {
            Drones.SetDefault();
            Beacons.SetDefault();
            Resources.SetDefault();
            Terrain.SetDefault();
            DronesCount = 100;
            StartingHorizontalPosition = 0.0f;
        }
    };

} // CW
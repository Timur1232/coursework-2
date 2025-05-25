#pragma once
#include "pch.h"

#include "TargetType.h"
#include "Drone.h"
#include "Beacon.h"
#include "Resource.h"
#include "TerrainGenerationSettings.h"

namespace CW {

    // для рендера
    struct SimulationState
    {
        // Drone
        std::vector<sf::Vector2f> DronesPositions;
        std::vector<sf::Angle> DronesDirections;

        // Beacon
        std::vector<sf::Vector2f> BeaconsPositions;
        std::vector<TargetType> BeaconsTypes;
        std::vector<float> BeaconsCharges;

        // Resource
        std::vector<sf::Vector2f> ResourcesPositions;
        std::vector<int> ResourcesAmounts;

        Terrain Terrain;

        // ResourceReciever
        sf::Vector2f ResieverPosition;

        // info
        int ResourceCount = 0;
        size_t DronesCount = 0;
        size_t BeconsCount = 0;


        void Clear()
        {
            DronesPositions.clear();
            DronesDirections.clear();
            BeaconsPositions.clear();
            BeaconsTypes.clear();
            BeaconsCharges.clear();
            ResourcesPositions.clear();
            ResourcesAmounts.clear();
            Terrain.TerrainSections.clear();
        }
    };

    // для сохранения
    struct FullSimulationState
    {
        std::vector<Drone> Drones;
        std::vector<Beacon> Beacons;
        std::vector<Resource> Resources;
        RecieverData RecieverData;

        SimulationSettings Settings;
        sf::Vector2i GeneratedRange;

        FullSimulationState() = default;
        void Clear()
        {
            Drones.clear();
            Beacons.clear();
            Resources.clear();
        }
    };

} // CW
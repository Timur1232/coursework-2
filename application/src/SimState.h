#pragma once
#include "pch.h"

#include "TargetType.h"
#include "Drone.h"
#include "Beacon.h"
#include "Resource.h"
#include "TerrainGenerationSettings.h"

namespace CW {

    // для синхронизации потоков
    struct SimulationState
    {
        // Drone
        std::vector<sf::Vector2f> DronesPositions;
        std::vector<sf::Angle> DronesDirections;
        std::vector<sf::Angle> DronesAttractions;

        // Beacon
        std::vector<sf::Vector2f> BeaconsPositions;
        std::vector<TargetType> BeaconsTypes;
        std::vector<float> BeaconsCharges;
        std::vector<sf::Vector2f> ChunksPositions;

        // Resource
        std::vector<sf::Vector2f> ResourcesPositions;
        std::vector<sf::Angle> ResourcesRotations;
        std::vector<int> ResourcesAmounts;

        Terrain Terrain;

        // ResourceReciever
        sf::Vector2f ResieverPosition;

        // info
        int ResourceCount = 0;
        int TotalResourceCount = 0;

        void Clear()
        {
            DronesPositions.clear();
            DronesDirections.clear();
            DronesAttractions.clear();

            BeaconsPositions.clear();
            BeaconsTypes.clear();
            BeaconsCharges.clear();
            ChunksPositions.clear();

            ResourcesPositions.clear();
            ResourcesRotations.clear();
            ResourcesAmounts.clear();

            Terrain.TerrainSections.clear();

            ResieverPosition = { 0.0f, 0.0f };

            ResourceCount = 0;
            TotalResourceCount = 0;
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
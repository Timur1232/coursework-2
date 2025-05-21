#pragma once
#include "pch.h"

#include "Drone.h"
#include "Beacon.h"
#include "Resource.h"
#include "Terrain.h"

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

        // ResourceReciever
        sf::Vector2f ResieverPosition;
        int ResourceCount;

        void Clear()
        {
            DronesPositions.clear();
            DronesDirections.clear();
            BeaconsPositions.clear();
            BeaconsTypes.clear();
            BeaconsCharges.clear();
            ResourcesPositions.clear();
            ResourcesAmounts.clear();
        }
    };

    // для сохранения
    struct FullSimulationState
    {
        std::vector<Drone> Drones;
        std::vector<Beacon> Beacons;
        std::vector<Resource> Resources;
        ResourceReciever Reciever;

        Terrain Terrain;
        sf::Vector2i GeneratedRange;

        void Clear()
        {
            Drones.clear();
            Beacons.clear();
            Resources.clear();
        }
    };

} // CW
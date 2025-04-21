#pragma once

#include <array>
#include <tuple>

#include "Drone.h"
#include "Beacon.h"
#include "Resource.h"

namespace CW {

    enum class ObjectPallete
    {
        None,
        Beacon,
        Drone,
        Resource
    };

    class ObjectPalleteBuilder
    {
    public:
        ObjectPalleteBuilder() = default;

        void UpdateInterface();
        ObjectPallete GetCurrentType() const;

        std::tuple<sf::Angle, TargetType> GetDroneComponents() const;
        TargetType GetBeaconType() const;
        int GetRsourceAmount() const;

    private:
        void settingsMenu();

    private:
        int m_CurrentIndex = 0;
        std::array<const char*, 4> m_ObjectsLabels = { "None", "Beacon", "Drone", "Resource"};

        // Drone
        TargetType m_DroneTarget = TargetType::Recource;
        float m_DroneAngle = 0.0f;

        // Beacon
        TargetType m_BeaconType = TargetType::Recource;

        // Resource
        int m_Amount = 10;
    };

} // CW
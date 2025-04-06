#include "Drone.h"

#include <ranges>
#include <imgui.h>

namespace CW {

    float Drone::s_Speed = 50.0f;
    float Drone::s_FOV = 0.2f;
    float Drone::s_BeaconCooldownSec = 5.0f;
    sf::Vector2f Drone::s_ViewDistanse = { 50.0f, 200.0f };

    Drone::Drone(sf::Vector2f position, sf::Vector2f direction)
        : m_Position(position), m_Direction(direction), m_MeshFOV(s_ViewDistanse.y)
    {
        m_DirectionVisual.setOrigin({ m_DirectionVisual.getRadius(), m_DirectionVisual.getRadius() });

        m_Mesh.setOrigin({ m_Mesh.getRadius(), m_Mesh.getRadius() });
        m_MeshFOV.setOrigin({ m_MeshFOV.getRadius(), m_MeshFOV.getRadius() });

        setMeshPos(position);

        m_Mesh.setFillColor(sf::Color::Yellow);
        m_MeshFOV.setFillColor(sf::Color::Transparent);
        m_MeshFOV.setOutlineColor(sf::Color::White);
        m_MeshFOV.setOutlineThickness(1.0f);
    }

    void Drone::update(sf::Time deltaTime)
    {
        ImGui::Begin("Drone");
        ImGui::Text("position: (%.2f, %.2f)", m_Position.x, m_Position.y);
        ImGui::Text("direction: (%.2f, %.2f)", m_Direction.x, m_Direction.y);
        ImGui::Text("beacon timer: %.1f s", m_BeaconTimerSec);
        ImGui::SliderFloat("drone fov", &s_FOV, 0.0f, 1.0f);
        ImGui::SliderFloat("drone speed", &s_Speed, 10.0f, 100.0f);
        ImGui::SliderFloat("beacon cooldown", &s_BeaconCooldownSec, 0.1f, 50.0f);
        ImGui::End();

        m_Position += m_Direction * s_Speed * deltaTime.asSeconds();
        setMeshPos(m_Position);

        m_BeaconTimerSec -= deltaTime.asSeconds();
        if (m_BeaconTimerSec <= 0)
        {
            CW_E::EventHandler::get().addEvent(CreateBeacon{ m_Position, oppositeBeaconType()});
            m_BeaconTimerSec = s_BeaconCooldownSec;
        }
    }

    void Drone::draw(sf::RenderWindow& render) const
    {
        render.draw(m_MeshFOV);
        render.draw(m_Mesh);
        render.draw(m_DirectionVisual);
    }

    void Drone::reactToBeacons(const std::vector<Beacon>& beacons)
    {
        const Beacon* furthestBeacon = nullptr;
        float furthestDist = -1.0f;

        auto filteredBeacons = beacons | std::views::filter([&](const Beacon& b) { return b.isAlive() && b.getType() == m_TargetBeaconType; });

        for (const auto& beacon : filteredBeacons)
        {
            if (auto positionDelta = beacon.getPos() - m_Position; (positionDelta.x != 0.0f || positionDelta.y != 0.0f) && m_Direction.dot(positionDelta.normalized()) >= s_FOV)
            {
                if (float dist = (beacon.getPos() - m_Position).length(); dist <= s_ViewDistanse.y && dist >= s_ViewDistanse.x && dist > furthestDist)
                {
                    furthestDist = dist;
                    furthestBeacon = &beacon;
                }
            }
        }

        if (furthestBeacon)
        {
            m_Direction = (furthestBeacon->getPos() - m_Position).normalized();
            m_DirectionVisual.setPosition(m_Position + m_Direction * 100.0f);
        }
    }

    BeaconType Drone::oppositeBeaconType() const
    {
        switch (m_TargetBeaconType)
        {
        case BeaconType::Navigation:    return BeaconType::Recource;
        case BeaconType::Recource:      return BeaconType::Navigation;
        default:                        return BeaconType::None;
        }
    }

    void Drone::setMeshPos(sf::Vector2f position)
    {
        m_Mesh.setPosition(position);
        m_MeshFOV.setPosition(position);
        m_DirectionVisual.setPosition(m_Position + m_Direction * 100.0f);
    }

} // CW
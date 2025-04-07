#include "Drone.h"

#include <ranges>
#include <imgui.h>
#include <random>

#include "utils/utils.h"

namespace CW {

    sf::CircleShape Drone::s_Mesh{ 50.0f };
    sf::CircleShape Drone::s_MeshFOV;
    sf::CircleShape Drone::s_DirectionVisual{ 4.0f, 3 };
    sf::CircleShape Drone::s_AttractionAngleVisual{ 4.0f, 3 };

    float Drone::s_Speed = 50.0f;
    sf::Angle Drone::s_TurningSpeed = sf::degrees(5.0f);

    float Drone::s_FOV = 0.2f;

    float Drone::s_BeaconCooldownSec = 5.0f;
    float Drone::s_WanderCooldownSec = 5.0f;

    sf::Angle Drone::s_RandomWanderAngle = sf::degrees(25.0f);
    sf::Angle Drone::s_WanderAngleThreshold = sf::degrees(0.5f);
    sf::Angle Drone::s_MaxTurningDelta = sf::degrees(30.0f);

    sf::Vector2f Drone::s_ViewDistanse = { 70.0f, 250.0f };

    Drone::Drone(sf::Vector2f position, sf::Angle directionAngle)
        : m_Position(position), m_DirectionAngle(directionAngle),
          m_AttractionAngle(m_DirectionAngle)
    {
        s_DirectionVisual.setOrigin({ s_DirectionVisual.getRadius(), s_DirectionVisual.getRadius() });
        s_AttractionAngleVisual.setOrigin({ s_AttractionAngleVisual.getRadius(), s_AttractionAngleVisual.getRadius() });
        s_AttractionAngleVisual.setFillColor(sf::Color::Blue);

        s_MeshFOV.setRadius(s_ViewDistanse.y);
        s_Mesh.setOrigin({ s_Mesh.getRadius(), s_Mesh.getRadius() });
        s_MeshFOV.setOrigin({ s_MeshFOV.getRadius(), s_MeshFOV.getRadius() });

        setMeshPos(position);

        s_Mesh.setFillColor(sf::Color::Yellow);
        s_MeshFOV.setFillColor(sf::Color::Transparent);
        s_MeshFOV.setOutlineColor(sf::Color::White);
        s_MeshFOV.setOutlineThickness(1.0f);
    }

    void Drone::update(sf::Time deltaTime)
    {
        ImGui::Begin("Drone");
        ImGui::Text("position: (%.2f, %.2f)", m_Position.x, m_Position.y);
        ImGui::Text("direction angle: %.2f", m_DirectionAngle.asDegrees());
        ImGui::Text("attraction angle: %.2f", m_AttractionAngle.asDegrees());
        ImGui::Text("beacon timer: %.1f s", m_BeaconTimerSec);
        ImGui::SliderFloat("drone fov", &s_FOV, 0.0f, 1.0f);
        ImGui::SliderFloat("drone speed", &s_Speed, 10.0f, 100.0f);
        ImGui::SliderFloat("beacon cooldown", &s_BeaconCooldownSec, 0.1f, 50.0f);
        ImGui::End();

        turn(deltaTime);
        m_Position += ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle) * s_Speed * deltaTime.asSeconds();

        m_BeaconTimerSec -= deltaTime.asSeconds();
        if (m_BeaconTimerSec <= 0)
        {
            CW_E::EventHandler::get().addEvent(CreateBeacon{ m_Position, opposite_target_type(m_TargetType)});
            m_BeaconTimerSec = s_BeaconCooldownSec;
        }

        wander(deltaTime);
    }

    void Drone::draw(sf::RenderWindow& render) const
    {
        setMeshPos(m_Position);
        render.draw(s_MeshFOV);
        render.draw(s_Mesh);
        render.draw(s_DirectionVisual);
        render.draw(s_AttractionAngleVisual);
    }

    void Drone::reactToBeacons(const std::vector<Beacon>& beacons)
    {
        const Beacon* furthestBeacon = nullptr;
        float furthestDist = -1.0f;

        auto filteredBeacons = beacons
            | std::views::filter([&](const Beacon& b) { return b.isAlive() && b.getType() == m_TargetType; });

        for (const auto& beacon : filteredBeacons)
        {
            if (auto positionDelta = beacon.getPos() - m_Position;
                (positionDelta.x != 0.0f || positionDelta.y != 0.0f)
                && ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle).dot(positionDelta.normalized()) >= s_FOV)
            {
                if (float dist = (beacon.getPos() - m_Position).length();
                    dist <= s_ViewDistanse.y && dist >= s_ViewDistanse.x && dist > furthestDist)
                {
                    furthestDist = dist;
                    furthestBeacon = &beacon;
                }
            }
        }

        if (furthestBeacon)
        {
            m_AttractionAngle = (furthestBeacon->getPos() - m_Position).angle();
            m_WanderTimer = s_WanderCooldownSec;
        }
    }

    void Drone::turn(sf::Time deltaTime)
    {
        sf::Angle delta;
        auto quarter = CW_E::angle::quarter(m_AttractionAngle);

        if ((quarter == CW_E::angle::Quarter::Second || quarter == CW_E::angle::Quarter::Third)
            && CW_E::opposite_signs(m_DirectionAngle.asRadians(), m_AttractionAngle.asRadians()))
        {
            if (quarter == CW_E::angle::Quarter::Second)
            {
                delta = m_AttractionAngle - sf::degrees(360.0f) + m_DirectionAngle;
            }
            else
            {
                delta = sf::degrees(360.0f) + m_AttractionAngle - m_DirectionAngle;
            }
        }
        else
        {
            delta = m_AttractionAngle - m_DirectionAngle;
        }

        delta = (std::clamp(
                delta + ((delta.asRadians() > 0) ? s_TurningSpeed : ((delta.asRadians() < 0) ? -s_TurningSpeed : sf::Angle::Zero)),
                -s_MaxTurningDelta, s_MaxTurningDelta
            ) * deltaTime.asSeconds());

        m_DirectionAngle = CW_E::loop(m_DirectionAngle, sf::degrees(-180.0f), sf::degrees(180.0f), delta);
    }

    void Drone::wander(sf::Time deltaTime)
    {
        static std::default_random_engine gen((unsigned int)std::time(0));
        static std::normal_distribution<float> normal(0.0f);

        if (m_WanderTimer > 0.0f)
        {
            m_WanderTimer -= deltaTime.asSeconds();
        }
        else if (std::abs((m_DirectionAngle - m_AttractionAngle).asRadians()) <= s_WanderAngleThreshold.asRadians())
        {
            m_AttractionAngle += s_RandomWanderAngle * normal(gen);
            m_WanderTimer = 0.0f;
        }
    }

    void Drone::setMeshPos(sf::Vector2f position) const
    {
        s_Mesh.setPosition(position);
        s_MeshFOV.setPosition(position);
        s_DirectionVisual.setPosition(m_Position + ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle) * 100.0f);
        s_DirectionVisual.setRotation(m_DirectionAngle + sf::degrees(90.0f));
        s_AttractionAngleVisual.setPosition(m_Position + ONE_LENGTH_VEC.rotatedBy(m_AttractionAngle) * 100.0f);
        s_AttractionAngleVisual.setRotation(m_AttractionAngle + sf::degrees(90.0f));
    }

} // CW
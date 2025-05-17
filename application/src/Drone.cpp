#include "pch.h"
#include "Drone.h"

#include "utils/utils.h"
#include "engine/Events/UserEvents.h"
#include "engine/Events/UserEventHandler.h"
#include "engine/Renderer.h"

#include "debug_utils/Profiler.h"
#include "debug_utils/Log.h"

#include "BitDirection.h"

namespace CW {

    static constexpr sf::Vector2f ONE_LENGTH_VEC = { 1.0f, 0.0f };

    DroneSettings::DroneSettings()
    {
        SetDefault();
    }

    void DroneSettings::SetDefault()
    {
        ViewDistance = { 70.0f, 250.0f };

        Speed = 100.0f;
        TurningSpeed = sf::degrees(15.0f);

        FOV = 0.5f;
        FOVRad = std::acos(FOV);

        PickupDist = 70.0f;

        BeaconCooldownSec = 5.0f;
        WanderCooldownSec = 5.0f;

        RandomWanderAngle = sf::degrees(25.0f);
        WanderAngleThreshold = sf::degrees(0.5f);
        MaxTurningDelta = sf::degrees(30.0f);
    }


    Drone::Drone(sf::Vector2f position, sf::Angle directionAngle, TargetType target)
        : Object(position), m_DirectionAngle(directionAngle.wrapSigned()),
          m_AttractionAngle(directionAngle.wrapSigned()),
          m_TargetType(target)
    {
    }

    void Drone::Update(float deltaTime, const DroneSettings& settings, std::vector<Resource>& resources)
    {
        CW_PROFILE_FUNCTION();
        turn(deltaTime, settings.TurningSpeed, settings.MaxTurningDelta);
        m_Position += ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle) * settings.Speed * deltaTime;

        if (m_Position.y < 250.0f)
        {
            if (m_DirectionAngle.asRadians() < 0.0f)
            {
                m_AttractionAngle = m_DirectionAngle * -1.0f;
            }
            if (m_Position.y < 0.0f)
            {
                m_Position.y = 0.0f;
            }
        }

        m_BeaconTimerSec -= deltaTime;
        if (m_BeaconTimerSec <= 0)
        {
            UserEventHandler::Get()
                .AddEvent(CreateBeacon{ m_Position, opposite_target_type(m_TargetType),
                    angle_to_bit_direction((m_DirectionAngle + sf::degrees(180.0f)).wrapSigned()) });
            m_BeaconTimerSec = settings.BeaconCooldownSec;
        }

        wander(deltaTime, settings.WanderAngleThreshold, settings.RandomWanderAngle, resources);

        if (m_TargetResourceIndex && resources.at(*m_TargetResourceIndex).IsCarried())
        {
            m_TargetResourceIndex = {};
        }
        else if (CheckResourceColission(settings.PickupDist, resources))
        {
            CW_TRACE("Drone picked {} resources on ({}, {})", resources.at(*m_TargetResourceIndex).GetResources(), resources.at(*m_TargetResourceIndex).GetPos().x, resources.at(*m_TargetResourceIndex).GetPos().y);
            m_TargetType = TargetType::Navigation;
            m_AttractionAngle = (m_AttractionAngle - sf::degrees(180.0f)).wrapSigned();
            m_DirectionAngle = m_AttractionAngle;
            m_CarriedResources = resources.at(*m_TargetResourceIndex).GetResources();
            resources.at(*m_TargetResourceIndex).Pickup();
            m_TargetResourceIndex = {};
        }
    }

    void Drone::ReactToBeacons(const ChunkHandler<Beacon>& beacons, float wanderCooldownSec, float FOV, sf::Vector2f viewDistance)
    {
        CW_PROFILE_FUNCTION();
        if (m_TargetResourceIndex)
            return;

        ChunkPtr<Beacon> chunk = beacons.GetAreaChunk(m_Position);

        auto [furthestBeacon, furthestDistSq] = findFurthestInChunk(chunk.CenterChunk, FOV, viewDistance);
        for (const Chunk<Beacon>* adjChunk : chunk.AdjacentChunks)
        {
            auto [furthestInChunk, furthestDistSqInChunk] = findFurthestInChunk(adjChunk, FOV, viewDistance);
            if (furthestInChunk && (!furthestBeacon || furthestDistSqInChunk < furthestDistSq))
            {
                furthestBeacon = furthestInChunk;
                furthestDistSq = furthestDistSqInChunk;
            }
        }

        if (furthestBeacon)
        {
            m_AttractionAngle = furthestBeacon->GetDirectionAngle();
            m_WanderTimer = wanderCooldownSec;
        }
    }

    bool Drone::ReactToResourceReciver(ResourceReciever& reciever, float wanderCooldownSec)
    {
        if (m_TargetType == TargetType::Navigation)
        {
            if (float distSq = distance_squared(reciever.GetPos(), m_Position);
                distSq <= reciever.GetRecieveRadius() * reciever.GetRecieveRadius())
            {
                reciever.AddResources(m_CarriedResources);
                m_CarriedResources = 0;
                m_TargetType = TargetType::Recource;
                m_AttractionAngle = (m_AttractionAngle - sf::degrees(180.0f)).wrapSigned();
                m_DirectionAngle = m_AttractionAngle;
                return true;
            }
            else if (distSq <= reciever.GetBroadcastRadius() * reciever.GetBroadcastRadius())
            {
                m_AttractionAngle = (reciever.GetPos() - m_Position).angle();
                m_WanderTimer = wanderCooldownSec;
                return true;
            }
        }
        return false;
    }

    void Drone::ReactToResources(std::vector<Resource>& resources, sf::Vector2f viewDistance, float FOV)
    {
        if (m_TargetType == TargetType::Recource && !resources.empty() && !m_TargetResourceIndex)
        {
            Resource* closestResource = nullptr;
            size_t closestResourceIndex = 0;
            float minDistSq = 0.0f;
            if (auto first = std::ranges::find_if(resources.begin(), resources.end(), [](const Resource& r) { return !r.IsCarried(); });
                first != resources.end())
            {
                closestResource = &(*first);
                minDistSq = distance_squared(closestResource->GetPos(), m_Position);
                closestResourceIndex = first - resources.begin();
            }
            else
            {
                return;
            }

            for (size_t i = closestResourceIndex + 1; i < resources.size(); ++i)
            {
                if (!resources[i].IsCarried())
                {
                    if (float otherDist = distance_squared(resources[i].GetPos(), m_Position);
                        otherDist < minDistSq)
                    {
                        minDistSq = otherDist;
                        closestResource = &resources[i];
                        closestResourceIndex = i;
                    }
                }
            }

            if (ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle).dot((closestResource->GetPos() - m_Position).normalized()) >= FOV
                && minDistSq >= viewDistance.x * viewDistance.x
                && minDistSq <= viewDistance.y * viewDistance.y)
            {
                m_TargetResourceIndex = closestResourceIndex;
                m_AttractionAngle = (closestResource->GetPos() - m_Position).angle();
                m_WanderTimer = 0.0f;
            }
        }
    }

    bool Drone::CheckResourceColission(float pickUpDist, const std::vector<Resource>& resources)
    {
        if (!m_TargetResourceIndex)
            return false;
        return distance_squared(resources.at(*m_TargetResourceIndex).GetPos(), m_Position) <= pickUpDist * pickUpDist;
    }

    void Drone::turn(float deltaTime, sf::Angle turningSpeed, sf::Angle maxTurningDelta)
    {
        sf::Angle deltaAngle;
        auto quarter = angle::quarter(m_AttractionAngle);

        if ((quarter == angle::Quarter::Second || quarter == angle::Quarter::Third)
            && opposite_signs(m_DirectionAngle.asRadians(), m_AttractionAngle.asRadians()))
        {
            if (quarter == angle::Quarter::Second)
            {
                deltaAngle = m_AttractionAngle - sf::degrees(360.0f) + m_DirectionAngle;
            }
            else
            {
                deltaAngle = sf::degrees(360.0f) + m_AttractionAngle - m_DirectionAngle;
            }
        }
        else
        {
            deltaAngle = m_AttractionAngle - m_DirectionAngle;
        }

        deltaAngle = (std::clamp(
                deltaAngle + ((deltaAngle.asRadians() > 0) ? turningSpeed : ((deltaAngle.asRadians() < 0) ? -turningSpeed : sf::Angle::Zero)),
                -maxTurningDelta, maxTurningDelta
            ) * deltaTime);

        m_DirectionAngle = loop(m_DirectionAngle, sf::degrees(-180.0f), sf::degrees(180.0f), deltaAngle);
    }

    void Drone::wander(float deltaTime, sf::Angle wanderAngleThreshold, sf::Angle randomWanderAngle, const std::vector<Resource>& resources)
    {
        if (m_WanderTimer > 0.0f)
        {
            m_WanderTimer -= deltaTime;
            return;
        }

        if (m_TargetResourceIndex)
        {
            m_AttractionAngle = (resources.at(*m_TargetResourceIndex).GetPos() - m_Position).angle();
            return;
        }
        else if (std::abs((m_DirectionAngle - m_AttractionAngle).asRadians()) <= wanderAngleThreshold.asRadians())
        {
            m_AttractionAngle += randomWanderAngle * ((float)std::rand() / RAND_MAX * 2.0f - 1.0f);
            m_AttractionAngle = m_AttractionAngle.wrapSigned();
            m_WanderTimer = 0.0f;
        }
    }

    std::pair<const Beacon*, float> Drone::findFurthestInChunk(
        const Chunk<Beacon>* chunk, float FOV, sf::Vector2f viewDistance)
    {
        if (!chunk)
            return { nullptr, -1.0f };

        const Beacon* furthestBeacon = nullptr;
        float furthestDistSq = -1.0f;

        auto filteredBeacons = *chunk
            | std::views::filter([&](const Indexed<Beacon>* b) { return b && (*b)->IsAlive() && (*b)->GetType() == m_TargetType; });

        for (const auto& ibeacon : filteredBeacons)
        {
            const auto& beacon = ibeacon->Object;
            if (auto positionDelta = beacon.GetPos() - m_Position;
                (positionDelta.x != 0.0f || positionDelta.y != 0.0f)
                && ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle).dot(positionDelta.normalized()) >= FOV)
            {
                if (float distSq = distance_squared(beacon.GetPos(), m_Position);
                    distSq <= viewDistance.y * viewDistance.y
                    && distSq >= viewDistance.x * viewDistance.x && distSq > furthestDistSq)
                {
                    furthestDistSq = distSq;
                    furthestBeacon = &beacon;
                }
            }
        }
        return { furthestBeacon, furthestDistSq };
    }


    DroneManager::DroneManager()
    {
        m_Texture = CreateUnique<sf::Texture>("res/sprites/drone_sprite.png");
        m_Sprite = CreateUnique<sf::Sprite>(*m_Texture);
        m_Sprite->setOrigin(static_cast<sf::Vector2f>(m_Texture->getSize()) / 2.0f);
    }

    void DroneManager::UpdateAllDrones(
        float deltaTime,
        std::vector<Resource>& resources,
        const ChunkHandler<Beacon>& beacons,
        ResourceReciever& reciever,
        const Terrain& terrain)
    {
        for (auto& drone : m_Drones)
        {
            drone.Update(deltaTime, m_DroneSettings, resources);

            // Подсчет максимальной достигнутой горизонтальной позиции
            if (drone.GetPos().x < m_FurthestHorizontalReach.x)
                m_FurthestHorizontalReach.x = drone.GetPos().x;
            if (drone.GetPos().x > m_FurthestHorizontalReach.y)
                m_FurthestHorizontalReach.y = drone.GetPos().x;

            if (terrain.IsNear(drone, 50.0f))
            {
                drone.SetDirection((drone.GetDirection() + sf::degrees(180.0f)).wrapSigned());
                drone.SetAttraction(drone.GetDirection());
            }

            drone.ReactToResources(resources, m_DroneSettings.ViewDistance, m_DroneSettings.FOV);

            if (!drone.ReactToResourceReciver(reciever, m_DroneSettings.WanderCooldownSec))
                drone.ReactToBeacons(beacons, m_DroneSettings.WanderCooldownSec, m_DroneSettings.FOV, m_DroneSettings.ViewDistance);
        }
    }

    void DroneManager::DrawAllDrones(sf::RenderWindow& render)
    {
        for (auto& drone : m_Drones)
        {
            debugDrawDirectionVisuals(drone.GetPos(), drone.GetDirection(), drone.GetAttraction());
            debugDrawViewDistance(drone.GetPos(), drone.GetDirection());

            m_Sprite->setPosition(drone.GetPos());
            if (abs(drone.GetDirection().asRadians()) > angle::PI_2)
            {
                m_Sprite->setScale({1.0f, -1.0f});
            }
            else
            {
                m_Sprite->setScale({ 1.0f, 1.0f });
            }
            m_Sprite->setRotation(drone.GetDirection());
            Renderer::Get().Draw(*m_Sprite);
        }
    }

    void DroneManager::Clear()
    {
        m_Drones.clear();
    }

    void DroneManager::Reset(size_t droneCount, sf::Vector2f startPosition, TargetType target)
    {
        Clear();
        m_Drones.reserve(droneCount);
        float angleStep = 2.0f * angle::PI / (float)droneCount;
        float angle = 0.0f;
        for (size_t i = 0; i < droneCount; ++i, angle += angleStep)
        {
            m_Drones.emplace_back(startPosition, sf::radians(angle), target);
        }
    }

    void DroneManager::CreateDrone(sf::Vector2f position, sf::Angle directionAngle, TargetType target)
    {
        m_Drones.emplace_back(position, directionAngle, target);
    }

    void DroneManager::InfoInterface(bool* open)
    {
        if (*open)
        {
            ImGui::Begin("Drones");
            size_t index = 0;
            for (const auto& drone : m_Drones)
            {
                ImGui::Separator();
                ImGui::Text("index: %d", index);
                ImGui::Text("position: (%.2f, %.2f)", drone.GetPos().x, drone.GetPos().y);
                ImGui::Text("direction angle: %.2f", drone.GetDirection().asDegrees());
                ImGui::Text("attraction angle: %.2f", drone.GetAttraction().asDegrees());
                ImGui::Text("beacon timer: %.1f s", drone.GetBeaconSpawnTimer());
                ImGui::Text("carried resources: %d", drone.GetCarriedResources());
                ImGui::Text(drone.GetTargetResourceIndex() ? "has target" : "no target");

                ++index;
            }
            ImGui::End();
        }
    }

    void DroneManager::DebugInterface()
    {
        ImGui::Checkbox("show view distance", &m_DrawViewDistance);
        ImGui::Checkbox("show direction", &m_DrawDirection);
        if (ImGui::SliderFloat("fov", &m_DroneSettings.FOV, 0.0f, 1.0f))
        {
            m_DroneSettings.FOVRad = std::acos(m_DroneSettings.FOV);
        }
        ImGui::SliderFloat("speed", &m_DroneSettings.Speed, 50.0f, 200.0f);

        static float tmp;
        tmp = m_DroneSettings.TurningSpeed.asRadians();
        if (ImGui::SliderAngle("turning speed", &tmp, 0.5f, 180.0f))
            m_DroneSettings.TurningSpeed = sf::radians(tmp);

        tmp = m_DroneSettings.RandomWanderAngle.asRadians();
        if (ImGui::SliderAngle("random wander angle", &tmp, 0.0f, 180.0f))
            m_DroneSettings.RandomWanderAngle = sf::radians(tmp);

        tmp = m_DroneSettings.WanderAngleThreshold.asRadians();
        if (ImGui::SliderAngle("wander angle threshold", &tmp, 0.01f, 10.0f))
            m_DroneSettings.WanderAngleThreshold = sf::radians(tmp);

        tmp = m_DroneSettings.MaxTurningDelta.asRadians();
        if (ImGui::SliderAngle("max turning delta", &tmp, 1.0f, 180.0f))
            m_DroneSettings.MaxTurningDelta = sf::radians(tmp);

        ImGui::SliderFloat2("view distanse", &m_DroneSettings.ViewDistance.x, 0.0f, 500.0f);

        ImGui::SliderFloat("beacon spawn cooldown", &m_DroneSettings.BeaconCooldownSec, 0.1f, 50.0f);
        ImGui::SliderFloat("wander cooldown", &m_DroneSettings.WanderCooldownSec, 0.1f, 50.0f);
    }

    void DroneManager::SetDefaultSettings()
    {
        m_DroneSettings.SetDefault();
        m_DrawViewDistance = false;
        m_DrawDirection = false;
    }

    inline void DroneManager::debugDrawDirectionVisuals(sf::Vector2f position, sf::Angle directionAngle, sf::Angle attractionAngle) const
    {
        if (m_DrawDirection)
        {
            auto& directionArrowBuilder = Renderer::Get().BeginCircleShape()
                .PointCount(3)
                .Radius(4.0f);
            directionArrowBuilder.Position(position + ONE_LENGTH_VEC.rotatedBy(directionAngle) * 100.0f)
                .Rotation(directionAngle + sf::degrees(90.0f))
                .Draw();
            directionArrowBuilder.Position(position + ONE_LENGTH_VEC.rotatedBy(attractionAngle) * 100.0f)
                .Rotation(attractionAngle + sf::degrees(90.0f))
                .Color(sf::Color::Blue)
                .Draw();
            directionArrowBuilder.SetDefault();
        }
    }

    inline void DroneManager::debugDrawViewDistance(sf::Vector2f position, sf::Angle directionAngle) const
    {
        if (m_DrawViewDistance)
        {
            auto& FOVVisualBuilder = Renderer::Get().BeginLineShape()
                .Length(m_DroneSettings.ViewDistance.y)
                .Position(position);
            FOVVisualBuilder
                .SetRotationByP1(sf::radians(m_DroneSettings.FOVRad))
                .RotateByP1(directionAngle)
                .Draw();
            FOVVisualBuilder.Position(position)
                .SetRotationByP1(sf::radians(-m_DroneSettings.FOVRad))
                .RotateByP1(directionAngle)
                .Draw();
            FOVVisualBuilder.SetDefault();
            
            auto& viewDistMeshBuilder = Renderer::Get().BeginCircleShape()
                .PointCount(16)
                .Color(sf::Color::Transparent)
                .OutlineColor(sf::Color::White)
                .OutlineThickness(1.0f);
            viewDistMeshBuilder
                .Radius(m_DroneSettings.ViewDistance.x)
                .Position(position)
                .Draw();
            viewDistMeshBuilder
                .Radius(m_DroneSettings.ViewDistance.y)
                .Position(position)
                .Draw();
            viewDistMeshBuilder.SetDefault();
        }
    }

} // CW
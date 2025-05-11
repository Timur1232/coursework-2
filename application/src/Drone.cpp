#include "pch.h"
#include "Drone.h"

#include "utils/utils.h"
#include "engine/Events.h"
#include "engine/UserDispatcher.h"

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

    void Drone::Update(sf::Time deltaTime, const DroneSettings& settings)
    {
        //CW_PROFILE_FUNCTION();
        turn(deltaTime, settings.TurningSpeed, settings.MaxTurningDelta);
        m_Position += ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle) * settings.Speed * deltaTime.asSeconds();

        m_BeaconTimerSec -= deltaTime.asSeconds();
        if (m_BeaconTimerSec <= 0)
        {
            UserEventHandler::Get()
                .AddEvent(CreateBeacon{ m_Position, opposite_target_type(m_TargetType),
                    angle_to_bit_direction((m_DirectionAngle + sf::degrees(180.0f)).wrapSigned()) });
            //CW_WARN("Drone\'s spawning beacons not implemented.");
            m_BeaconTimerSec = settings.BeaconCooldownSec;
        }

        wander(deltaTime, settings.WanderAngleThreshold, settings.RandomWanderAngle);

        if (m_TargetResource && m_TargetResource->IsCarried())
        {
            m_TargetResource = nullptr;
        }
        else if (CheckResourceColission(settings.PickupDist))
        {
            CW_TRACE("Drone picked {} resources on ({}, {})", m_TargetResource->GetResources(), m_TargetResource->GetPos().x, m_TargetResource->GetPos().y);
            m_TargetType = TargetType::Navigation;
            m_AttractionAngle = (m_AttractionAngle - sf::degrees(180.0f)).wrapSigned();
            m_DirectionAngle = m_AttractionAngle;
            m_CarriedResources = m_TargetResource->GetResources();
            m_TargetResource->Pickup();
            m_TargetResource = nullptr;
        }
    }

    void Drone::ReactToBeacons(const ChunkHandler<Beacon>& beacons, float wanderCooldownSec, float FOV, sf::Vector2f viewDistance)
    {
        //CW_PROFILE_FUNCTION();
        if (m_TargetResource)
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
            //m_AttractionAngle = (furthestBeacon->GetPos() - m_Position).angle();
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

    void Drone::ReactToResources(std::vector<std::unique_ptr<Resource>>& resources, sf::Vector2f viewDistance, float FOV)
    {
        if (m_TargetType == TargetType::Recource && !resources.empty() && !m_TargetResource)
        {
            auto validResources = resources
                | std::ranges::views::filter([](const std::unique_ptr<Resource>& r) { return !r->IsCarried(); });

            if (std::ranges::empty(validResources))
            {
                return;
            }

            Resource* closestResource = validResources.front().get();
            float minDistSq = distance_squared(closestResource->GetPos(), m_Position);

            for (auto& resource : validResources | std::ranges::views::drop(1))
            {
                if (float otherDist = distance_squared(resource->GetPos(), m_Position);
                    otherDist < minDistSq)
                {
                    minDistSq = otherDist;
                    closestResource = resource.get();
                }
            }

            if (ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle).dot((closestResource->GetPos() - m_Position).normalized()) >= FOV
                && minDistSq >= viewDistance.x * viewDistance.x
                && minDistSq <= viewDistance.y * viewDistance.y)
            {
                m_TargetResource = &(*closestResource);
                m_AttractionAngle = (m_TargetResource->GetPos() - m_Position).angle();
                m_WanderTimer = 0.0f;
            }
        }
    }

    bool Drone::CheckResourceColission(float pickUpDist)
    {
        if (!m_TargetResource)
            return false;
        return distance_squared(m_TargetResource->GetPos(), m_Position) <= pickUpDist * pickUpDist;
    }

    void Drone::turn(sf::Time deltaTime, sf::Angle turningSpeed, sf::Angle maxTurningDelta)
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
            ) * deltaTime.asSeconds());

        m_DirectionAngle = loop(m_DirectionAngle, sf::degrees(-180.0f), sf::degrees(180.0f), deltaAngle);
    }

    void Drone::wander(sf::Time deltaTime, sf::Angle wanderAngleThreshold, sf::Angle randomWanderAngle)
    {
        if (m_WanderTimer > 0.0f)
        {
            m_WanderTimer -= deltaTime.asSeconds();
            return;
        }

        if (m_TargetResource)
        {
            m_AttractionAngle = (m_TargetResource->GetPos() - m_Position).angle();
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
            | std::views::filter([&](const Beacon* b) { return b && b->IsAlive() && b->GetType() == m_TargetType; });

        for (const auto& beacon : filteredBeacons)
        {
            if (auto positionDelta = beacon->GetPos() - m_Position;
                (positionDelta.x != 0.0f || positionDelta.y != 0.0f)
                && ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle).dot(positionDelta.normalized()) >= FOV)
            {
                if (float distSq = distance_squared(beacon->GetPos(), m_Position);
                    distSq <= viewDistance.y * viewDistance.y
                    && distSq >= viewDistance.x * viewDistance.x && distSq > furthestDistSq)
                {
                    furthestDistSq = distSq;
                    furthestBeacon = beacon;
                }
            }
        }
        return { furthestBeacon, furthestDistSq };
    }


    void DroneManager::UpdateAllDrones(
        sf::Time deltaTime,
        std::vector<std::unique_ptr<Resource>>& resources,
        const ChunkHandler<Beacon>& beacons,
        ResourceReciever& reciever,
        const Terrain& terrain)
    {
        for (auto& drone : m_Drones)
        {
            drone.Update(deltaTime, m_DroneSettings);
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
            setMeshPos(drone.GetPos(), drone.GetDirection(), drone.GetAttraction());
            if (m_DrawViewDistance)
            {
                m_MeshViewDistance.setPosition(drone.GetPos());

                m_MeshViewDistance.setRadius(m_DroneSettings.ViewDistance.x);
                m_MeshViewDistance.setOrigin({ m_DroneSettings.ViewDistance.x, m_DroneSettings.ViewDistance.x });

                render.draw(m_MeshViewDistance);

                m_MeshViewDistance.setRadius(m_DroneSettings.ViewDistance.y);
                m_MeshViewDistance.setOrigin({ m_DroneSettings.ViewDistance.y, m_DroneSettings.ViewDistance.y });

                render.draw(m_MeshViewDistance);

                render.draw(m_FOVVisual[0]);
                render.draw(m_FOVVisual[1]);
            }

            render.draw(m_Mesh);

            if (m_DrawDirection)
            {
                render.draw(m_DirectionVisual);
                render.draw(m_AttractionAngleVisual);
            }
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
                if (auto resource = drone.GetTargetResource())
                    ImGui::Text("target position: (%.2f, %.2f)", resource->GetPos().x, resource->GetPos().y);
                else
                    ImGui::Text("no target");

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
            m_DroneSettings.FOVRad = std::acos(m_DroneSettings.FOV);
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

        m_Mesh.setRadius(50.0f);
        m_Mesh.setOrigin(m_Mesh.getGeometricCenter());
        m_Mesh.setPointCount(16);
        
        m_MeshViewDistance.setPointCount(16);
        
        m_DirectionVisual.setRadius(4.0f);
        m_DirectionVisual.setOrigin(m_DirectionVisual.getGeometricCenter());
        m_DirectionVisual.setPointCount(3);
        
        m_AttractionAngleVisual.setRadius(4.0f);
        m_AttractionAngleVisual.setPointCount(3);
        m_AttractionAngleVisual.setOrigin(m_AttractionAngleVisual.getGeometricCenter());
        
        m_Mesh.setFillColor(sf::Color::Yellow);
        m_AttractionAngleVisual.setFillColor(sf::Color::Blue);
        m_MeshViewDistance.setFillColor(sf::Color::Transparent);
        m_MeshViewDistance.setOutlineColor(sf::Color::White);
        m_MeshViewDistance.setOutlineThickness(1.0f);
        
        m_FOVVisual[0].SetLength(m_DroneSettings.ViewDistance.y);
        m_FOVVisual[1].SetLength(m_DroneSettings.ViewDistance.y);
        
        m_DrawViewDistance = false;
        m_DrawDirection = false;
    }

    void DroneManager::setMeshPos(sf::Vector2f position, sf::Angle directionAngle, sf::Angle attractionAngle)
    {
        m_Mesh.setPosition(position);

        if (m_DrawDirection)
        {
            m_DirectionVisual.setPosition(position + ONE_LENGTH_VEC.rotatedBy(directionAngle) * 100.0f);
            m_DirectionVisual.setRotation(directionAngle + sf::degrees(90.0f));
            m_AttractionAngleVisual.setPosition(position + ONE_LENGTH_VEC.rotatedBy(attractionAngle) * 100.0f);
            m_AttractionAngleVisual.setRotation(attractionAngle + sf::degrees(90.0f));
        }
        if (m_DrawViewDistance)
        {
            m_FOVVisual[0].SetPosition(position);
            m_FOVVisual[1].SetPosition(position);
            
            m_FOVVisual[0].SetRotation(directionAngle);
            m_FOVVisual[1].SetRotation(directionAngle);
            
            m_FOVVisual[0].RotateByPoint1(sf::radians(m_DroneSettings.FOVRad));
            m_FOVVisual[1].RotateByPoint1(sf::radians(-m_DroneSettings.FOVRad));
        }
    }

} // CW
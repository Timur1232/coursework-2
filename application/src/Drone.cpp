#include "pch.h"
#include "Drone.h"

#include "utils/utils.h"
#include "engine/Events/UserEvents.h"
#include "engine/Events/UserEventHandler.h"
#include "engine/Renderer.h"

#include "debug_utils/Profiler.h"
#include "debug_utils/Log.h"

#include "BitDirection.h"
#include "SimState.h"

namespace CW {

    DroneSettings::DroneSettings()
    {
        SetDefault();
    }

    void DroneSettings::SetDefault()
    {
        BitDirections = true;

        ViewDistance = { 70.0f, 250.0f };

        DischargeRate = 0.05f;

        Speed = 100.0f;
        TurningSpeed = angle::to_radians(15.0f);

        FOV = 0.5f;

        PickupDist = 70.0f;

        BeaconCooldownSec = 5.0f;
        WanderCooldownSec = 5.0f;
        SpawnDroneCooldownSec = 20.0f;

        RandomWanderAngleRad = angle::to_radians(25.0f);
        WanderAngleThresholdDeg = 0.5f;
        MaxTurningDeltaRad = angle::to_radians(30.0f);

        DroneCost = 10;
        DroneSpawnCooldown = 25.0f;
        BasePosition = sf::Vector2f(0.0f, 0.0f);
    }


    Drone::Drone(sf::Vector2f position, sf::Angle directionAngle, TargetType target)
        : Object(position), m_DirectionAngle(directionAngle.wrapSigned()),
          m_AttractionAngle(directionAngle.wrapSigned()),
          m_TargetType(target)
    {
    }

    void Drone::WriteToFile(std::ofstream& file) const
    {
        file.write(reinterpret_cast<const char*>(&m_Position), sizeof(m_Position));
        file.write(reinterpret_cast<const char*>(&m_Charge), sizeof(m_Charge));

        float directionRad = m_DirectionAngle.asRadians();
        float attractionRad = m_AttractionAngle.asRadians();
        file.write(reinterpret_cast<const char*>(&directionRad), sizeof(directionRad));
        file.write(reinterpret_cast<const char*>(&attractionRad), sizeof(attractionRad));

        file.write(reinterpret_cast<const char*>(&m_CarriedResources), sizeof(m_CarriedResources));
        file.write(reinterpret_cast<const char*>(&m_BeaconTimerSec), sizeof(m_BeaconTimerSec));
        file.write(reinterpret_cast<const char*>(&m_WanderTimer), sizeof(m_WanderTimer));

        int typeInt = static_cast<int>(m_TargetType);
        file.write(reinterpret_cast<const char*>(&typeInt), sizeof(typeInt));
    }

    void Drone::ReadFromFile(std::ifstream& file)
    {
        file.read(reinterpret_cast<char*>(&m_Position), sizeof(sf::Vector2f));
        file.read(reinterpret_cast<char*>(&m_Charge), sizeof(m_Charge));

        float directionRad, attractionRad;
        file.read(reinterpret_cast<char*>(&directionRad), sizeof(directionRad));
        file.read(reinterpret_cast<char*>(&attractionRad), sizeof(attractionRad));
        m_DirectionAngle = sf::radians(directionRad);
        m_AttractionAngle = sf::radians(attractionRad);

        file.read(reinterpret_cast<char*>(&m_CarriedResources), sizeof(m_CarriedResources));
        file.read(reinterpret_cast<char*>(&m_BeaconTimerSec), sizeof(m_BeaconTimerSec));
        file.read(reinterpret_cast<char*>(&m_WanderTimer), sizeof(m_WanderTimer));

        int typeInt;
        file.read(reinterpret_cast<char*>(&typeInt), sizeof(typeInt));
        m_TargetType = static_cast<TargetType>(typeInt);
    }

    void Drone::Revive(sf::Vector2f position, sf::Angle direction, TargetType targetType)
    {
        m_Position = position;
        m_DirectionAngle = m_AttractionAngle = direction;
        m_Charge = 1.0f;
        m_TargetType = targetType;
        m_TargetResourceIndex = {};
        m_CarriedResources = 0;
        m_BeaconTimerSec = 0.0f;
        m_WanderTimer = 0.0f;
        m_IsAlive = true;
    }

    bool Drone::Update(float deltaTime, const DroneSettings& settings, std::vector<Resource>& resources, BeaconComponents& components)
    {
        CW_PROFILE_FUNCTION();
        if (!IsAlive())
            return false;
        m_Charge -= settings.DischargeRate / 100.0f * deltaTime;
        if (m_Charge <= 0.0f)
        {
            m_IsAlive = false;
            m_Charge = 0.0f;
            return false;
        }
        
        bool beaconSpawn = false;

        turn(deltaTime, sf::radians(settings.TurningSpeed), sf::radians(settings.MaxTurningDeltaRad));
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
            beaconSpawn = true;
            components = { m_Position, opposite_target_type(m_TargetType),
                    (m_DirectionAngle + sf::degrees(180.0f)).wrapSigned() };
            m_BeaconTimerSec = settings.BeaconCooldownSec;
        }

        wander(deltaTime, sf::degrees(settings.WanderAngleThresholdDeg), sf::radians(settings.RandomWanderAngleRad), resources);

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

        return beaconSpawn;
    }

    void Drone::ReactToBeacons(const ChunkHandler<Beacon>& beacons, float wanderCooldownSec, float FOV, sf::Vector2f viewDistance, bool bitDir)
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
            if (bitDir)
                m_AttractionAngle = furthestBeacon->GetBitDirectionAngle();
            else
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
                m_Charge = 1.0f;
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
            if (float distSq = distance_squared(beacon.GetPos(), m_Position);
                distSq <= viewDistance.y * viewDistance.y
                && distSq >= viewDistance.x * viewDistance.x && distSq > furthestDistSq)
            {
                if (auto positionDelta = beacon.GetPos() - m_Position;
                    (positionDelta.x != 0.0f || positionDelta.y != 0.0f)
                    && ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle).dot(positionDelta.normalized()) >= FOV)
                {
                    furthestDistSq = distSq;
                    furthestBeacon = &beacon;
                }
            }
        }
        return { furthestBeacon, furthestDistSq };
    }

    //======================================[DroneManager]======================================//

    DroneManager::DroneManager()
    {
        SetSettings(m_DroneSettings);
    }

    DroneManager::DroneManager(const DroneSettings& settings)
    {
        SetSettings(settings);
    }

    void DroneManager::SetState(FullSimulationState& state)
    {
        m_DroneSettings = state.Settings.Drones;
        m_Drones = std::move(state.Drones);
        for (auto& drone : m_Drones)
        {
            drone.ResetTarget();
        }
    }

    void DroneManager::CollectState(SimulationState& state) const
    {
        for (const auto& drone : m_Drones)
        {
            if (drone.IsAlive())
            {
                state.DronesPositions.push_back(drone.GetPos());
                state.DronesDirections.push_back(drone.GetDirection());
            }
        }
    }

    void DroneManager::CollectState(FullSimulationState& state) const
    {
        for (const auto& drone : m_Drones)
        {
            if (drone.IsAlive())
            {
                state.Drones.push_back(drone);
            }
        }
    }

    void DroneManager::SetSettings(const DroneSettings& settings)
    {
        m_DroneSettings = settings;
        m_FOVRadPrecalc = std::acos(settings.FOV);
    }

    std::vector<BeaconComponents> DroneManager::UpdateAllDrones(
        float deltaTime,
        std::vector<Resource>& resources,
        const ChunkHandler<Beacon>& beacons,
        ResourceReciever& reciever,
        const TerrainGenerator& terrain)
    {
        BeaconComponents components;
        std::vector<BeaconComponents> componentsVec;
        for (size_t i = 0; i < m_Drones.size() - m_DeadDrones; ++i)
        {
            auto& drone = m_Drones[i];
            if (drone.Update(deltaTime, m_DroneSettings, resources, components))
            {
                componentsVec.emplace_back(components);
            }
            if (!drone.IsAlive())
            {
                ++m_DeadDrones;
                auto& last = m_Drones[m_Drones.size() - m_DeadDrones];
                std::swap(drone, last);
                --i;
                continue;
            }

            // Подсчет максимальной достигнутой горизонтальной позиции
            if (drone.GetPos().x < m_FurthestHorizontalReach.x)
                m_FurthestHorizontalReach.x = drone.GetPos().x;
            if (drone.GetPos().x > m_FurthestHorizontalReach.y)
                m_FurthestHorizontalReach.y = drone.GetPos().x;

            if (terrain.IsNear(drone, 80.0f))
            {
                drone.SetDirection((drone.GetDirection() + sf::degrees(180.0f)).wrapSigned());
                drone.SetAttraction(drone.GetDirection());
                sf::Vector2f pos = drone.GetPos();
                drone.SetPos({pos.x, pos.y - 15.0f});
            }

            drone.ReactToResources(resources, m_DroneSettings.ViewDistance, m_DroneSettings.FOV);

            if (!drone.ReactToResourceReciver(reciever, m_DroneSettings.WanderCooldownSec))
                drone.ReactToBeacons(beacons, m_DroneSettings.WanderCooldownSec, m_DroneSettings.FOV, m_DroneSettings.ViewDistance, m_DroneSettings.BitDirections);
        }
        return componentsVec;
    }

    void DroneManager::Clear()
    {
        m_Drones.clear();
    }

    void DroneManager::Reset(size_t droneCount, sf::Vector2f startPosition, TargetType target)
    {
        Clear();
        m_Drones.reserve(droneCount);
        float angleStep = angle::PI / (float) droneCount;
        float angle = 0.0f;
        for (size_t i = 0; i < droneCount; ++i, angle += angleStep)
        {
            m_Drones.emplace_back(startPosition, sf::radians(angle), target);
        }
    }

    void DroneManager::CreateDrone(sf::Vector2f position, sf::Angle directionAngle, TargetType target)
    {
        if (m_DeadDrones)
        {
            auto& drone= m_Drones[m_Drones.size() - m_DeadDrones];
            drone.Revive(position, directionAngle, target);
            --m_DeadDrones;
        }
        else
        {
            if (m_Drones.size() == m_Drones.capacity())
            {
                CW_INFO("Drones realloc");
            }
            m_Drones.emplace_back(position, directionAngle, target);
        }
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
        /*ImGui::Checkbox("show view distance", &m_DrawViewDistance);
        ImGui::Checkbox("show direction", &m_DrawDirection);
        if (ImGui::SliderFloat("fov", &m_DroneSettings.FOV, 0.0f, 1.0f))
        {
            m_FOVRadPrecalc = std::acos(m_DroneSettings.FOV);
        }
        ImGui::SliderFloat("speed", &m_DroneSettings.Speed, 50.0f, 200.0f);

        static float tmp;
        tmp = m_DroneSettings.TurningSpeed.asRadians();
        if (ImGui::SliderAngle("turning speed", &tmp, 0.5f, 180.0f))
            m_DroneSettings.TurningSpeed = sf::radians(tmp);

        tmp = m_DroneSettings.RandomWanderAngleRad.asRadians();
        if (ImGui::SliderAngle("random wander angle", &tmp, 0.0f, 180.0f))
            m_DroneSettings.RandomWanderAngleRad = sf::radians(tmp);

        tmp = m_DroneSettings.WanderAngleThresholdDeg.asRadians();
        if (ImGui::SliderAngle("wander angle threshold", &tmp, 0.01f, 10.0f))
            m_DroneSettings.WanderAngleThresholdDeg = sf::radians(tmp);

        tmp = m_DroneSettings.MaxTurningDeltaRad.asRadians();
        if (ImGui::SliderAngle("max turning delta", &tmp, 1.0f, 180.0f))
            m_DroneSettings.MaxTurningDeltaRad = sf::radians(tmp);

        ImGui::SliderFloat2("view distanse", &m_DroneSettings.ViewDistance.x, 0.0f, 500.0f);

        ImGui::SliderFloat("beacon spawn cooldown", &m_DroneSettings.BeaconCooldownSec, 0.1f, 50.0f);
        ImGui::SliderFloat("wander cooldown", &m_DroneSettings.WanderCooldownSec, 0.1f, 50.0f);*/
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
                .SetRotationByP1(sf::radians(m_FOVRadPrecalc))
                .RotateByP1(directionAngle)
                .Draw();
            FOVVisualBuilder.Position(position)
                .SetRotationByP1(sf::radians(-m_FOVRadPrecalc))
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
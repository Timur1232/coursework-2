#include "pch.h"
#include "Drone.h"

#include "utils/utils.h"
#include "engine/Events.h"

#include "debug_utils/Profiler.h"
#include "debug_utils/Log.h"

namespace CW {

    sf::CircleShape Drone::s_Mesh;
    sf::CircleShape Drone::s_MeshViewDistance;
    sf::CircleShape Drone::s_DirectionVisual;
    sf::CircleShape Drone::s_AttractionAngleVisual;

    float Drone::s_Speed;
    sf::Angle Drone::s_TurningSpeed;

    float Drone::s_FOV;

    float Drone::s_PickupDist;

    float Drone::s_BeaconCooldownSec;
    float Drone::s_WanderCooldownSec;

    sf::Angle Drone::s_RandomWanderAngle;
    sf::Angle Drone::s_WanderAngleThreshold ;
    sf::Angle Drone::s_MaxTurningDelta;

    sf::Vector2f Drone::s_ViewDistanse;

    bool Drone::s_DrawViewDistance;
    bool Drone::s_DrawDirection;

    Drone::Drone(sf::Vector2f position, sf::Angle directionAngle, TargetType target)
        : m_Position(position), m_DirectionAngle(directionAngle.wrapSigned()),
          m_AttractionAngle(directionAngle.wrapSigned()),
          m_TargetType(target)
    {
        setMeshPos(position);
    }

    void Drone::StaticInit()
    {
        s_Mesh.setRadius(50.0f);
        s_Mesh.setOrigin({ s_Mesh.getRadius(), s_Mesh.getRadius() });
        s_Mesh.setPointCount(16);

        s_MeshViewDistance.setPointCount(16);

        s_DirectionVisual.setRadius(4.0f);
        s_DirectionVisual.setOrigin({ s_DirectionVisual.getRadius(), s_DirectionVisual.getRadius() });
        s_DirectionVisual.setPointCount(3);

        s_AttractionAngleVisual.setRadius(4.0f);
        s_AttractionAngleVisual.setPointCount(3);
        s_AttractionAngleVisual.setOrigin({ s_AttractionAngleVisual.getRadius(), s_AttractionAngleVisual.getRadius() });

        s_Mesh.setFillColor(sf::Color::Yellow);
        s_AttractionAngleVisual.setFillColor(sf::Color::Blue);
        s_MeshViewDistance.setFillColor(sf::Color::Transparent);
        s_MeshViewDistance.setOutlineColor(sf::Color::White);
        s_MeshViewDistance.setOutlineThickness(1.0f);

        s_Speed = 50.0f;
        s_TurningSpeed = sf::degrees(5.0f);

        s_FOV = 0.5f;

        s_PickupDist = 70.0f;

        s_BeaconCooldownSec = 5.0f;
        s_WanderCooldownSec = 5.0f;

        s_RandomWanderAngle = sf::degrees(25.0f);
        s_WanderAngleThreshold = sf::degrees(0.5f);
        s_MaxTurningDelta = sf::degrees(30.0f);

        s_ViewDistanse = { 70.0f, 250.0f };

        s_DrawViewDistance = false;
        s_DrawDirection = false;
    }

    void Drone::DebugInterface()
    {
        ImGui::Checkbox("show view distance", &s_DrawViewDistance);
        ImGui::Checkbox("show direction", &s_DrawDirection);
        ImGui::SliderFloat("fov", &s_FOV, 0.0f, 1.0f);
        ImGui::SliderFloat("speed", &s_Speed, 10.0f, 100.0f);
        
        static float tmp;
        tmp = s_TurningSpeed.asRadians();
        if (ImGui::SliderAngle("turning speed", &tmp, 0.5f, 180.0f))
            s_TurningSpeed = sf::radians(tmp);
        
        tmp = s_RandomWanderAngle.asRadians();
        if (ImGui::SliderAngle("random wander angle", &tmp, 0.0f, 180.0f))
            s_RandomWanderAngle = sf::radians(tmp);
        
        tmp = s_WanderAngleThreshold.asRadians();
        if (ImGui::SliderAngle("wander angle threshold", &tmp, 0.01f, 10.0f))
            s_WanderAngleThreshold = sf::radians(tmp);
        
        tmp = s_MaxTurningDelta.asRadians();
        if (ImGui::SliderAngle("max turning delta", &tmp, 1.0f, 180.0f))
            s_MaxTurningDelta = sf::radians(tmp);
        
        ImGui::SliderFloat2("view distanse", &s_ViewDistanse.x, 0.0f, 1000.0f);
        
        ImGui::SliderFloat("beacon spawn cooldown", &s_BeaconCooldownSec, 0.1f, 50.0f);
        ImGui::SliderFloat("wander cooldown", &s_WanderCooldownSec, 0.1f, 50.0f);
    }

    void Drone::InfoInterface(size_t index, bool* open) const
    {
        ImGui::Begin("Drones", open);
        ImGui::Separator();
        ImGui::Text("index: %d", index);
        ImGui::Text("position: (%.2f, %.2f)", m_Position.x, m_Position.y);
        ImGui::Text("direction angle: %.2f", m_DirectionAngle.asDegrees());
        ImGui::Text("attraction angle: %.2f", m_AttractionAngle.asDegrees());
        ImGui::Text("beacon timer: %.1f s", m_BeaconTimerSec);
        ImGui::Text("carried resources: %d", m_CarriedResources);
        ImGui::End();
    }

    void Drone::Update(sf::Time deltaTime)
    {
        //CW_PROFILE_FUNCTION();
        turn(deltaTime);
        m_Position += ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle) * s_Speed * deltaTime.asSeconds();

        m_BeaconTimerSec -= deltaTime.asSeconds();
        if (m_BeaconTimerSec <= 0)
        {
            EventHandler::Get().AddEvent(CreateBeacon{ m_Position, opposite_target_type(m_TargetType)});
            m_BeaconTimerSec = s_BeaconCooldownSec;
        }

        wander(deltaTime);

        if (CheckResourceColission())
        {
            CW_TRACE("Drone picked {} resources on ({}, {})", m_TargetResource->GetResources(), m_TargetResource->GetPos().x, m_TargetResource->GetPos().y);
            m_TargetType = TargetType::Navigation;
            m_AttractionAngle = (m_AttractionAngle - sf::degrees(180.0f)).wrapSigned();
            m_DirectionAngle = m_AttractionAngle;
            m_CarriedResources = m_TargetResource->GetResources();
            m_TargetResource->PickUp();
        }
    }

    void Drone::Draw(sf::RenderWindow& render) const
    {
        setMeshPos(m_Position);

        if (s_DrawViewDistance)
        {
            s_MeshViewDistance.setPosition(m_Position);

            s_MeshViewDistance.setRadius(s_ViewDistanse.x);
            s_MeshViewDistance.setOrigin({ s_ViewDistanse.x, s_ViewDistanse.x });

            render.draw(s_MeshViewDistance);

            s_MeshViewDistance.setRadius(s_ViewDistanse.y);
            s_MeshViewDistance.setOrigin({ s_ViewDistanse.y, s_ViewDistanse.y });

            render.draw(s_MeshViewDistance);
        }

        render.draw(s_Mesh);

        if (s_DrawDirection)
        {
            render.draw(s_DirectionVisual);
            render.draw(s_AttractionAngleVisual);
        }
    }

    void Drone::ReactToBeacons(const std::vector<Beacon*>& beacons)
    {
        //CW_PROFILE_FUNCTION();
        const Beacon* furthestBeacon = nullptr;
        float furthestDist = -1.0f;

        auto filteredBeacons = beacons
            | std::views::filter([&](const Beacon* b) { return b->IsAlive() && b->GetType() == m_TargetType; });

        for (const auto& beacon : filteredBeacons)
        {
            if (auto positionDelta = beacon->GetPos() - m_Position;
                (positionDelta.x != 0.0f || positionDelta.y != 0.0f)
                && ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle).dot(positionDelta.normalized()) >= s_FOV)
            {
                if (float distSq = (beacon->GetPos() - m_Position).length();
                    distSq <= s_ViewDistanse.y && distSq >= s_ViewDistanse.x && distSq > furthestDist)
                {
                    furthestDist = distSq;
                    furthestBeacon = beacon;
                }
            }
        }

        if (furthestBeacon)
        {
            m_AttractionAngle = (furthestBeacon->GetPos() - m_Position).angle();
            m_WanderTimer = s_WanderCooldownSec;
        }
    }

    bool Drone::ReactToResourceReciver(ResourceReciever& reciever)
    {
        if (m_TargetType == TargetType::Navigation)
        {
            if (float distSq = distance_squared(reciever.GetPos(), m_Position);
                distSq <= reciever.GetRecieveRadius() * reciever.GetRecieveRadius())
            {
                reciever.AddResources(10);
                m_TargetType = TargetType::Recource;
                m_AttractionAngle = (m_AttractionAngle - sf::degrees(180.0f)).wrapSigned();
                m_DirectionAngle = m_AttractionAngle;
                return true;
            }
            else if (distSq <= reciever.GetBroadcastRadius() * reciever.GetBroadcastRadius())
            {
                m_AttractionAngle = (reciever.GetPos() - m_Position).angle();
                m_WanderTimer = s_WanderCooldownSec;
                return true;
            }
        }
        return false;
    }

    void Drone::ReactToResources(std::vector<Resource>& resources)
    {
        if (m_TargetType == TargetType::Recource && !resources.empty() && !m_TargetResource)
        {
            // TODO: переделать - дрон должен выбрать ресурс один раз и плыть к нему, чтобы поднять 
            auto found = resources.begin();
            float minDist = distance_squared(found->GetPos(), m_Position);
            auto iter = resources.begin() + 1;
            while (iter != resources.end())
            {
                if (!iter->IsCarried())
                {
                    if (float otherDist = distance_squared(iter->GetPos(), m_Position);
                        otherDist < minDist)
                    {
                        minDist = otherDist;
                        found = iter;
                    }
                }
                ++iter;
            }
            if (found->IsCarried())
            {
                return;
            }
            m_TargetResource = &(*found);
            m_AttractionAngle = (m_TargetResource->GetPos() - m_Position).angle().wrapSigned();

            // TODO: сначала дрон должен двигаться к ресурсу
            
        }
        return;
    }

    bool Drone::CheckResourceColission()
    {
        return !m_TargetResource 
            && distance_squared(m_TargetResource->GetPos(), m_Position) <= s_PickupDist * s_PickupDist;
    }

    void Drone::turn(sf::Time deltaTime)
    {
        sf::Angle delta;
        auto quarter = angle::quarter(m_AttractionAngle);

        if ((quarter == angle::Quarter::Second || quarter == angle::Quarter::Third)
            && opposite_signs(m_DirectionAngle.asRadians(), m_AttractionAngle.asRadians()))
        {
            if (quarter == angle::Quarter::Second)
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

        m_DirectionAngle = loop(m_DirectionAngle, sf::degrees(-180.0f), sf::degrees(180.0f), delta);
    }

    void Drone::wander(sf::Time deltaTime)
    {
        /*static std::default_random_engine gen((unsigned int)std::time(0));
        static std::normal_distribution<float> normal(0.0f);*/

        if (m_WanderTimer > 0.0f)
        {
            m_WanderTimer -= deltaTime.asSeconds();
        }
        else if (std::abs((m_DirectionAngle - m_AttractionAngle).asRadians()) <= s_WanderAngleThreshold.asRadians())
        {
            //m_AttractionAngle += s_RandomWanderAngle * normal(gen);
            m_AttractionAngle += s_RandomWanderAngle * ((float)std::rand() / RAND_MAX * 2.0f - 1.0f);
            m_AttractionAngle = m_AttractionAngle.wrapSigned();
            m_WanderTimer = 0.0f;
        }
    }

    void Drone::setMeshPos(sf::Vector2f position) const
    {
        s_Mesh.setPosition(position);

        if (s_DrawDirection)
        {
            s_DirectionVisual.setPosition(m_Position + ONE_LENGTH_VEC.rotatedBy(m_DirectionAngle) * 100.0f);
            s_DirectionVisual.setRotation(m_DirectionAngle + sf::degrees(90.0f));
            s_AttractionAngleVisual.setPosition(m_Position + ONE_LENGTH_VEC.rotatedBy(m_AttractionAngle) * 100.0f);
            s_AttractionAngleVisual.setRotation(m_AttractionAngle + sf::degrees(90.0f));
        }
    }


    //void DroneDebugInterface::updateInterface() const
    //{
    //    ImGui::Begin("Drone");
    //    ImGui::SliderFloat("fov", &Drone::s_FOV, 0.0f, 1.0f);
    //    ImGui::SliderFloat("speed", &Drone::s_Speed, 10.0f, 100.0f);

    //    {
    //        //float tmp = Drone::s_TurningSpeed.asDegrees();
    //        ImGui::SliderAngle("turning speed", (float*)(&Drone::s_TurningSpeed), 0.5f, 180.0f);
    //        //Drone::s_TurningSpeed = sf::degrees(tmp);

    //        ImGui::SliderAngle("random wander angle", (float*)(&Drone::s_RandomWanderAngle), 0.0f, 180.0f);
    //        ImGui::SliderAngle("wander angle threshold", (float*)(&Drone::s_WanderAngleThreshold), 0.01f, 10.0f);
    //        ImGui::SliderAngle("max turning delta", (float*)(&Drone::s_MaxTurningDelta), 1.0f, 180.0f);
    //    }

    //    ImGui::SliderFloat2("view distanse", &Drone::s_ViewDistanse.x, 0.0f, 1000.0f);

    //    ImGui::SliderFloat("beacon spawn cooldown", &Drone::s_BeaconCooldownSec, 0.1f, 50.0f);
    //    ImGui::SliderFloat("wander cooldown", &Drone::s_WanderCooldownSec, 0.1f, 50.0f);
    //    ImGui::End();
    //}

} // CW
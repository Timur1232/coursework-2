#include "pch.h"

#include <engine/EntryPoint.h>

#include <engine/ProgramCore.h>
#include "engine/Chunks.h"

#include "debug_utils/Log.h"
#include "debug_utils/Profiler.h"
#include "utils/ArenaAllocator.h"
#include "ObjectPallete.h"

#include "Camera2D.h"
#include "Beacon.h"
#include "Drone.h"

#include "BitDirection.h"

#include "Terrain.h"

namespace CW {


    class MyApp
        : public Application,
          public KeyPressedObs,
          public ClosedObs,
          public MouseButtonPressedObs,
          public MouseButtonReleasedObs,
          public CreateBeaconObs
    {
    public:
        MyApp()
            : Application(800, 600, "test"),
              m_Camera(0, 0, 800, 600)
        {
            m_Camera.SubscribeOnEvents();
            m_Resources.reserve(128);

            Drone::StaticInit();
            Resource::StaticInit();

            m_ChunkMesh.setSize({ 500.0f, 500.0f });
            m_ChunkMesh.setFillColor(sf::Color::Transparent);
            m_ChunkMesh.setOutlineThickness(2.0f);
            m_ChunkMesh.setOutlineColor({ 255, 255, 255, 180 });

            for (int i = -5; i <= 5; ++i)
                m_Terrain.Generate(i);
        }

        void Update(sf::Time deltaTime) override
        {
            CW_PROFILE_FUNCTION();
            m_Camera.Update(deltaTime);

            {
                CW_PROFILE_SCOPE("beacons update");
                m_BeaconManager.Update(deltaTime);
            }

            {
                CW_PROFILE_SCOPE("drones update");
                size_t index = 0;
                for (auto& drone : m_Drones)
                {
                    drone.Update(deltaTime);

                    drone.ReactToResources(m_Resources);

                    if (!drone.ReactToResourceReciver(m_ResourceReciever))
                        drone.ReactToBeacons(m_BeaconManager.GetChuncks());

                    if (m_DronesInfo)
                        drone.InfoInterface(index, &m_DronesInfo);
                    ++index;
                }
            }
        }

        void PauseUpdate(sf::Time deltaTime) override
        {
            m_Camera.Update(deltaTime);
        }

        void Draw(sf::RenderWindow& render) override
        {
            CW_PROFILE_FUNCTION();
            render.setView(m_Camera.GetView());

            if (m_DrawChunks)
            {
                m_ChunkMesh.setOutlineThickness(m_Camera.GetZoomFactor());
                for (const auto& [key, _] : m_BeaconManager.GetChuncks().GetAllChunks())
                {
                    sf::Vector2f chunkPos = static_cast<sf::Vector2f>(key) * 500.0f;
                    m_ChunkMesh.setPosition(chunkPos);
                    render.draw(m_ChunkMesh);
                }
            }

            m_ResourceReciever.Draw(render);
            
            for (auto& resource : m_Resources)
            {
                resource.Draw(render);
            }

            if (m_DrawBeacons)
            {
                CW_PROFILE_SCOPE("beacons draw");
                m_BeaconManager.DrawAllBeacons(render);
            }

            {
                CW_PROFILE_SCOPE("drones draw");
                for (auto& drone : m_Drones)
                {
                    drone.Draw(render);
                }
            }

            m_Terrain.SetDotScale(m_Camera.GetZoomFactor());
            m_Terrain.Draw(render);
        }

        void OnClosed() override
        {
            Close();
        }

        void OnKeyPressed(const sf::Event::KeyPressed* event) override
        {
            if (event->code == sf::Keyboard::Key::Space)
            {
                SwitchPause();
            }
        }

        void OnMouseButtonPressed(const sf::Event::MouseButtonPressed* e) override
        {
            if (!m_Hold && !ImGui::GetIO().WantCaptureMouse && e->button == sf::Mouse::Button::Left)
            {
                switch (m_ObjPallete.GetCurrentType())
                {
                case ObjectPallete::Beacon:
                {
                    m_BeaconManager.CreateBeacon(m_Camera.WorldPosition(e->position), m_ObjPallete.GetBeaconType(), 0);
                    break;
                }
                case ObjectPallete::Drone:
                {
                    auto [direction, targetType] = m_ObjPallete.GetDroneComponents();
                    m_Drones.emplace_back(m_Camera.WorldPosition(e->position), direction, targetType);
                    break;
                }
                case ObjectPallete::Resource:
                {
                    m_Resources.emplace_back(m_Camera.WorldPosition(e->position), m_ObjPallete.GetRsourceAmount());
                    break;
                }
                }
                m_Hold = true;
            }
        }

        void OnMouseButtonReleased(const sf::Event::MouseButtonReleased* e) override
        {
            m_Hold = m_Hold && e->button != sf::Mouse::Button::Left;
        }

        void OnCreateBeacon(const CreateBeacon* e) override
        {
            m_BeaconManager.CreateBeacon(e->Position, e->Type, e->BitDirection);
        }

        void RestartSim(size_t droneCount, sf::Vector2f startPosition = { 0.0f, 0.0f }, TargetType target = TargetType::Recource)
        {
            m_BeaconManager.Clear();

            m_Drones.clear();
            m_Drones.reserve(droneCount);
            float angleStep = 2.0f * angle::PI / (float)droneCount;
            float angle = 0.0f;
            for (size_t i = 0; i < droneCount; ++i, angle += angleStep)
            {
                m_Drones.emplace_back(startPosition, sf::radians(angle), target);
            }

            m_Resources.clear();
            CW_TRACE("Restarting simulation with {} drones", droneCount);
        }

        void UpdateInterface() override
        {
            ImGui::Begin("Debug");
            if (ImGui::CollapsingHeader("App-statistics"))
            {
                ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("paused: %d", IsPaused());
                ImGui::Spacing();
                ImGui::Text("m_Beacons size: %d", m_BeaconManager.Size());
                ImGui::Text("m_Beacons capasity: %d", m_BeaconManager.Capacity());
                ImGui::Spacing();
                ImGui::Text("mouse hovering on any window: %d", ImGui::GetIO().WantCaptureMouse);
                m_Camera.DebugInterface();

                ImGui::Spacing();
                ImGui::Text("chunks amount: %d", m_BeaconManager.GetChuncks().Size());
                ImGui::Checkbox("draw chunks (dangerous!)", &m_DrawChunks);
            }

            if (ImGui::CollapsingHeader("Object-pallete"))
            {
                m_ObjPallete.UpdateInterface();
            }

            if (ImGui::CollapsingHeader("Resources"))
            {
                m_ResourceReciever.DebugInterface();
            }

            if (ImGui::CollapsingHeader("Drones"))
            {
                static int droneCount = 100;
                static sf::Vector2f startPos = { 0.0f, 0.0f };
                static TargetType target = TargetType::Recource;

                ImGui::InputInt("drone count", &droneCount);
                ImGui::InputFloat2("starting position", &startPos.x);
                ImGui::Text("target on start");
                if (ImGui::RadioButton("Recource##Drones", target == TargetType::Recource))
                    target = TargetType::Recource;
                ImGui::SameLine();
                if (ImGui::RadioButton("Navigation##Drones", target == TargetType::Navigation))
                    target = TargetType::Navigation;

                if (ImGui::Button("restart simulation"))
                {
                    RestartSim(droneCount, startPos, target);
                }

                ImGui::Checkbox("show drones info", &m_DronesInfo);
                Drone::DebugInterface();

                ImGui::Text("default drones settings");
                if (ImGui::Button("default drone"))
                    Drone::StaticInit();
            }

            if (ImGui::CollapsingHeader("Beacons"))
            {
                ImGui::Text("default beacons settings");
                if (ImGui::Button("default beacon"))
                    CW_WARN("Not implemented");

                ImGui::Checkbox("draw beacons", &m_DrawBeacons);
                ImGui::Checkbox("show beacons info", &m_BeaconsInfo);
                m_BeaconManager.DebugInterface();
            }
            ImGui::End();

            if (m_BeaconsInfo)
                m_BeaconManager.InfoInterface(&m_BeaconsInfo);
        }

    private:
        Camera2D m_Camera;
        bool m_Hold = false;

        BeaconManager m_BeaconManager;

        std::vector<Drone> m_Drones;

        ResourceReciever m_ResourceReciever{ {0.0f, 0.0f} };
        std::vector<Resource> m_Resources;

        Terrain m_Terrain;

        // Debug
        bool m_BeaconsInfo = false;
        bool m_DronesInfo = false;
        bool m_DrawBeacons = true;

        ObjectPalleteBuilder m_ObjPallete;

        bool m_DrawChunks = false;
        mutable sf::RectangleShape m_ChunkMesh;

    };

} // CW

std::unique_ptr<CW::Application> create_program(int argc, const char** argv)
{
    return std::make_unique<CW::MyApp>();
}
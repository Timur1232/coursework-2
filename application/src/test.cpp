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
            m_Resources.Reserve(128);

            m_Drones.SetDefaultSettings();

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
                m_Beacons.Update(deltaTime);
            }

            {
                CW_PROFILE_SCOPE("drones update");
                m_Drones.UpdateAllDrones(deltaTime, m_Resources.GetResources(), m_Beacons.GetChuncks(), m_ResourceReciever, m_Terrain);
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
                for (const auto& chunk : m_Beacons.GetChuncks().GetAllChunks())
                {
                    sf::Vector2f chunkPos = static_cast<sf::Vector2f>(chunk.GetKey()) * 500.0f;
                    m_ChunkMesh.setPosition(chunkPos);
                    render.draw(m_ChunkMesh);
                }
            }

            m_ResourceReciever.Draw(render);
            
            m_Resources.DrawAllRecources(render);

            if (m_DrawBeacons)
            {
                CW_PROFILE_SCOPE("beacons draw");
                m_Beacons.DrawAllBeacons(render);
            }

            {
                CW_PROFILE_SCOPE("drones draw");
                m_Drones.DrawAllDrones(render);
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
                    m_Beacons.CreateBeacon(m_Camera.WorldPosition(e->position), m_ObjPallete.GetBeaconType(), 0);
                    break;
                }
                case ObjectPallete::Drone:
                {
                    auto [direction, targetType] = m_ObjPallete.GetDroneComponents();
                    m_Drones.CreateDrone(m_Camera.WorldPosition(e->position), direction, targetType);
                    break;
                }
                case ObjectPallete::Resource:
                {
                    m_Resources.CreateResource(m_Camera.WorldPosition(e->position), m_ObjPallete.GetRsourceAmount());
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
            m_Beacons.CreateBeacon(e->Position, e->Type, e->BitDirection);
        }

        void RestartSim(size_t droneCount, sf::Vector2f startPosition = { 0.0f, 0.0f }, TargetType target = TargetType::Recource)
        {
            m_Beacons.Clear();
            m_Drones.Reset(droneCount, startPosition, target);
            m_Resources.Clear();
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
                ImGui::Text("m_Beacons size: %d", m_Beacons.Size());
                ImGui::Text("m_Beacons capasity: %d", m_Beacons.Capacity());
                ImGui::Spacing();
                ImGui::Text("mouse hovering on any window: %d", ImGui::GetIO().WantCaptureMouse);
                m_Camera.DebugInterface();

                ImGui::Spacing();
                ImGui::Text("chunks amount: %d", m_Beacons.GetChuncks().Size());
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
                m_Drones.DebugInterface();

                ImGui::Text("default drones settings");
                if (ImGui::Button("default drone"))
                    m_Drones.SetDefaultSettings();
            }

            if (ImGui::CollapsingHeader("Beacons"))
            {
                ImGui::Text("default beacons settings");
                if (ImGui::Button("default beacon"))
                    CW_WARN("Not implemented");

                ImGui::Checkbox("draw beacons", &m_DrawBeacons);
                ImGui::Checkbox("show beacons info", &m_BeaconsInfo);
                m_Beacons.DebugInterface();
            }

            if (ImGui::CollapsingHeader("Terrain"))
            {
                ImGui::Text("regenerate terrain");
                if (ImGui::Button("regenerate"))
                    m_Terrain.RegenerateExisting();

                static float yOffset = m_Terrain.GetYOffset();
                if (ImGui::InputFloat("y offset", &yOffset))
                    m_Terrain.SetYOffset(yOffset);

                static int samplesCount = (int)m_Terrain.GetSamplesPerSection();
                static float sectionWidth = m_Terrain.GetSectionWidth();
                if (ImGui::InputInt("samples per section", &samplesCount))
                    m_Terrain.SetSamplesPerSection(samplesCount);
                if (ImGui::InputFloat("section width", &sectionWidth))
                    m_Terrain.SetSectionWidth(sectionWidth);

                if (ImGui::TreeNode("noise settings"))
                {
                    static int seed = m_Terrain.GetSeed();
                    static float maxHeight = m_Terrain.GetMaxHeight();
                    static float mappedNoiseDistance = m_Terrain.GetMappedNoiseDistance();
                    if (ImGui::InputInt("seed", &seed))
                        m_Terrain.SetSeed(seed);
                    if (ImGui::InputFloat("max height", &maxHeight))
                        m_Terrain.SetMaxHeight(maxHeight);
                    if (ImGui::InputFloat("mapped noise distance", &mappedNoiseDistance))
                        m_Terrain.SetMapedNoiseDistance(mappedNoiseDistance);

                    ImGui::Spacing();

                    static float gain = m_Terrain.GetGain();
                    static float weightedStrength = m_Terrain.GetWeightedStrength();
                    static int octaves = m_Terrain.GetOctaves();
                    static float lacunarity = m_Terrain.GetLacunarity();
                    if (ImGui::InputFloat("gain", &gain))
                        m_Terrain.SetGain(gain);
                    if (ImGui::InputFloat("weighted strength", &weightedStrength))
                        m_Terrain.SetWeightedStrength(weightedStrength);
                    if (ImGui::InputInt("octaves", &octaves))
                        m_Terrain.SetOctaves(octaves);
                    if (ImGui::InputFloat("lacunarity", &lacunarity))
                        m_Terrain.SetLacunarity(lacunarity);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("generating chunks"))
                {
                    static int startIndex = 0;
                    static int endIndex = 0;
                    ImGui::InputInt("start", &startIndex);
                    ImGui::InputInt("end", &endIndex);
                    if (ImGui::Button("generate"))
                    {
                        if (endIndex < startIndex)
                        {
                            CW_WARN("Need start <= end indecies");
                        }
                        else
                        {
                            for (int i = startIndex; i < endIndex; ++i)
                            {
                                m_Terrain.Generate(i);
                            }
                        }
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::End();

            if (m_BeaconsInfo)
                m_Beacons.InfoInterface(&m_BeaconsInfo);

            if (m_DronesInfo)
                m_Drones.InfoInterface(&m_DronesInfo);
        }

    private:
        Camera2D m_Camera;
        bool m_Hold = false;

        BeaconManager m_Beacons;
        DroneManager m_Drones;
        ResourceManager m_Resources;

        ResourceReciever m_ResourceReciever{ {0.0f, 0.0f} };

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
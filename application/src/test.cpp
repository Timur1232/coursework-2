#include "pch.h"

#include "engine/EntryPoint.h"
#include "engine/ProgramCore.h"
#include "engine/Events/CoreEvents.h"
#include "engine/Events/UserEvents.h"
#include "engine/Renderer.h"
#include "engine/Events/UserEventHandler.h"

#include "debug_utils/Log.h"
#include "debug_utils/Profiler.h"
#include "utils/ArenaAllocator.h"
#include "ObjectPallete.h"

#include "Camera2D.h"
#include "Beacon.h"
#include "Drone.h"

#include "BitDirection.h"

#include "Terrain.h"

#include "engine/Button.h"

#define SHADERS_FOLDER "res/shaders/"
#define SPRITES_FOLDER "res/sprites/"

namespace CW {

    class SimulationLayer
        : public Layer
    {
    public:
        SimulationLayer(sf::Vector2f windowSize)
            : m_Camera(0, 0, windowSize.x, windowSize.y),
              m_WindowSize(windowSize)
        {
            m_Resources.Reserve(128);
            m_Drones.SetDefaultSettings();

            if (!m_WaterShader.loadFromFile(SHADERS_FOLDER "water_fragment.glsl", sf::Shader::Type::Fragment))
            {
                CW_ERROR("Unable to load water shader!");
            }
            if (!m_DarkeningShader.loadFromFile(SHADERS_FOLDER "deepness_darkening_fragment.glsl", sf::Shader::Type::Fragment))
            {
                CW_ERROR("Unable to load darkening shader!");
            }

            if (!m_TerrainTexture.loadFromFile(SPRITES_FOLDER "terrain_texture.png"))
            {
                CW_ERROR("Unable to load terrain texture!");
            }
            else
            {
                m_TerrainTexture.setRepeated(true);
            }

            GenerateChunkRange(m_GeneratedRange.x, m_GeneratedRange.y);

            m_WaterShader.setUniform("uResolution", windowSize);
            m_WaterShader.setUniform("uDeepDarkFactor", 3.5f);
            m_WaterShader.setUniform("uWaterYOffset", 300.0f);

            m_DarkeningShader.setUniform("uResolution", windowSize);
            m_DarkeningShader.setUniform("uDarkeningFactor", 25.0f);
            m_DarkeningShader.setUniform("uYOffset", 3000.0f);
        }

        void Update(float deltaTime) override
        {
            GenerateChunksInCameraView();
            GenerateChunksForDrones();

            UpdateInterface();
            {
                CW_PROFILE_SCOPE("beacons update");
                m_Beacons.Update(deltaTime);
            }
            {
                CW_PROFILE_SCOPE("drones update");
                m_Drones.UpdateAllDrones(deltaTime, m_Resources.GetResources(), m_Beacons.GetChuncks(), m_ResourceReciever, m_Terrain);
            }
            m_ElapsedTime += deltaTime;
        }

        void OnEvent(Event& event) override
        {
            EventDispatcher dispatcher(event);
            dispatcher.Dispach<MouseButtonPressed>(CW_BUILD_EVENT_FUNC(OnMouseButtonPressed));
            dispatcher.Dispach<MouseButtonReleased>(CW_BUILD_EVENT_FUNC(OnMouseButtonReleased));
            dispatcher.Dispach<WindowResized>(CW_BUILD_EVENT_FUNC(OnWindowResized));

            dispatcher.Dispach<CreateBeacon>(CW_BUILD_EVENT_FUNC(OnCreateBeacon));
            m_Camera.OnEvent(event);
        }

        void Draw() override
        {
            m_WaterShader.setUniform("uCameraPosition", m_Camera.GetView().getCenter());
            m_WaterShader.setUniform("uZoomFactor", m_Camera.GetZoomFactor());
            m_WaterShader.setUniform("uTime", m_ElapsedTime);

            m_DarkeningShader.setUniform("uZoomFactor", m_Camera.GetZoomFactor());
            m_DarkeningShader.setUniform("uCameraPosition", m_Camera.GetView().getCenter());
            m_DarkeningShader.setUniform("uCameraViewYSize", m_Camera.GetView().getSize().y);

            auto& renderer = Renderer::Get();
            renderer.ApplyDefaultView();
            renderer.BeginRectangleShape()
                .DefaultAfterDraw()
                .Size(m_WindowSize)
                .Shader(&m_WaterShader)
                .Draw();
            renderer.SetView(m_Camera.GetView());

            if (m_DrawChunks)
            {
                auto& chunkMeshBuilder = renderer.BeginRectangleShape();
                chunkMeshBuilder.Size({ 500.0f, 500.0f })
                    .Color(sf::Color::Transparent)
                    .OutlineThickness(m_Camera.GetZoomFactor())
                    .OutlineColor({ 255, 255, 255, 180 });
                for (const auto& chunk : m_Beacons.GetChuncks().GetAllChunks())
                {
                    sf::Vector2f chunkPos = static_cast<sf::Vector2f>(chunk.GetKey()) * 500.0f;
                    chunkMeshBuilder.Position(chunkPos)
                        .Draw();
                }
                chunkMeshBuilder.SetDefault();
            }

            m_ResourceReciever.Draw();

            m_Resources.DrawAllRecources();

            if (m_DrawBeacons)
            {
                CW_PROFILE_SCOPE("beacons draw");
                m_Beacons.DrawAllBeacons();
            }

            {
                CW_PROFILE_SCOPE("drones draw");
                m_Drones.DrawAllDrones();
            }

            for (const auto& mesh : m_TerrainSectionMeshes)
            {
                renderer.Draw(mesh);
                renderer.Draw(mesh, &m_DarkeningShader);
            }
        }

    private:
        bool OnMouseButtonPressed(MouseButtonPressed& e)
        {
            if (!m_Hold && !ImGui::GetIO().WantCaptureMouse && e.Data.button == sf::Mouse::Button::Left)
            {
                switch (m_ObjPallete.GetCurrentType())
                {
                case ObjectPallete::Beacon:
                {
                    auto [type, bitDir] = m_ObjPallete.GetBeaconComponents();
                    m_Beacons.CreateBeacon(m_Camera.PixelToWorldPosition(e.Data.position), type, bitDir);
                    break;
                }
                case ObjectPallete::Drone:
                {
                    auto [direction, targetType] = m_ObjPallete.GetDroneComponents();
                    m_Drones.CreateDrone(m_Camera.PixelToWorldPosition(e.Data.position), direction, targetType);
                    break;
                }
                case ObjectPallete::Resource:
                {
                    m_Resources.CreateResource(m_Camera.PixelToWorldPosition(e.Data.position), m_ObjPallete.GetRsourceAmount());
                    break;
                }
                }
                m_Hold = true;
            }
            return false;
        }

        bool OnMouseButtonReleased(MouseButtonReleased& e)
        {
            m_Hold = m_Hold && e.Data.button != sf::Mouse::Button::Left;
            return false;
        }

        bool OnWindowResized(WindowResized& e)
        {
            m_WaterShader.setUniform("uResolution", static_cast<sf::Vector2f>(e.Size));
            m_WindowSize = static_cast<sf::Vector2f>(e.Size);
            return false;
        }

        bool OnCreateBeacon(CreateBeacon& e)
        {
            m_Beacons.CreateBeacon(e.Position, e.Type, e.BitDirection);
            return false;
        }

        void RestartSim(size_t droneCount, sf::Vector2f startPosition = { 0.0f, 0.0f }, TargetType target = TargetType::Recource)
        {
            m_Beacons.Clear();
            m_Drones.Reset(droneCount, startPosition, target);
            m_Resources.Clear();
            CW_TRACE("Restarting simulation with {} drones", droneCount);
        }
        
        void UpdateInterface() 
        {
            ImGui::Begin("Debug");
            if (ImGui::CollapsingHeader("Simulation-info"))
            {
                ImGui::Text("m_Beacons size: %d", m_Beacons.Size());
                ImGui::Text("m_Beacons capasity: %d", m_Beacons.Capacity());
                ImGui::Spacing();
                ImGui::Text("mouse hovering on any window: %d", ImGui::GetIO().WantCaptureMouse);
                m_Camera.DebugInterface();

                ImGui::Spacing();
                ImGui::Text("chunks amount: %d", m_Beacons.GetChuncks().Size());
                ImGui::Checkbox("draw chunks", &m_DrawChunks);

                ImGui::Spacing();
                ImGui::Text("terrain sections generated: %d (%d; %d)", m_Terrain.GetSectionsCount(), m_GeneratedRange.x, m_GeneratedRange.y);
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
                        m_Terrain.SetDetailedSeed(seed);
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

        void GenerateChunksInCameraView()
        {
            sf::Vector2f cameraViewSize = m_Camera.GetView().getSize();
            sf::Vector2f cameraViewPosition = m_Camera.GetView().getCenter() - cameraViewSize / 2.0f;
            sf::FloatRect cameraViewRect{ cameraViewPosition, m_Camera.GetView().getSize() };

            int leftBorderKey = m_Terrain.CalcSectionKeyPosition(cameraViewPosition.x);
            int rightBorderKey = m_Terrain.CalcSectionKeyPosition(cameraViewPosition.x + cameraViewSize.x);

            float leftBorderX = m_Terrain.CalcSectionStartPosition(leftBorderKey) + m_Terrain.GetSectionWidth();
            float rightBorderX = m_Terrain.CalcSectionStartPosition(rightBorderKey);

            if (leftBorderKey < m_GeneratedRange.x && cameraViewRect.contains({ leftBorderX, cameraViewRect.getCenter().y }))
            {
                GenerateChunkRange(leftBorderKey, m_GeneratedRange.x);
                m_GeneratedRange.x = leftBorderKey;
            }
            if (rightBorderKey >= m_GeneratedRange.y && cameraViewRect.contains({ rightBorderX, cameraViewRect.getCenter().y }))
            {
                rightBorderKey += 1;
                GenerateChunkRange(m_GeneratedRange.y, rightBorderKey);
                m_GeneratedRange.y = rightBorderKey;
            }
        }

        void GenerateChunkRange(int left, int right)
        {
            for (int key = left; key < right; ++key)
            {
                if (!m_Terrain.Generate(key))
                    continue;
                auto& mesh = m_TerrainSectionMeshes.emplace_back();
                m_Terrain.GenerateMesh(mesh, key);
                mesh.setTexture(&m_TerrainTexture);
                mesh.setTextureRect({ {0, 0}, static_cast<sf::Vector2i>(mesh.getLocalBounds().size) });

                m_Resources.GenerateResourceOnSection(m_Terrain, key);
            }
        }

        void GenerateChunksForDrones()
        {
            auto [leftBorderX, rightBorderX] = m_Drones.GetFurthestHorizontalReach();
            int leftBorderKey = m_Terrain.CalcSectionKeyPosition(leftBorderX - m_Terrain.GetSectionWidth());
            int rightBorderKey = m_Terrain.CalcSectionKeyPosition(rightBorderX + m_Terrain.GetSectionWidth());
            if (leftBorderKey < m_GeneratedRange.x)
            {
                GenerateChunkRange(leftBorderKey, m_GeneratedRange.x);
                m_GeneratedRange.x = leftBorderKey;
            }
            if (rightBorderKey >= m_GeneratedRange.y)
            {
                rightBorderKey += 1;
                GenerateChunkRange(m_GeneratedRange.y, rightBorderKey);
                m_GeneratedRange.y = rightBorderKey;
            }
        }

    private:
        Camera2D m_Camera;
        bool m_Hold = false;

        BeaconManager m_Beacons;
        DroneManager m_Drones;
        ResourceManager m_Resources;
        ResourceReciever m_ResourceReciever{ {0.0f, 0.0f} };

        Terrain m_Terrain;
        sf::Vector2i m_GeneratedRange{-5, 5};

        std::vector<sf::ConvexShape> m_TerrainSectionMeshes;
        sf::Texture m_TerrainTexture;

        sf::Vector2f m_WindowSize;
        float m_ElapsedTime = 0.0f;

        sf::Shader m_WaterShader;
        sf::Shader m_DarkeningShader;

        // Debug
        bool m_BeaconsInfo = false;
        bool m_DronesInfo = false;
        bool m_DrawBeacons = true;

        ObjectPalleteBuilder m_ObjPallete;

        bool m_DrawChunks = false;
    };


    class MainMenuLayer
        : public Layer
    {
    public:
        MainMenuLayer(sf::Vector2f windowSize)
            : m_StartButtonCollision(CreateShared<RectCollision>(sf::Vector2f{ windowSize.x / 2.0f - 50.0f, windowSize.y / 2.0f - 100.0f }, sf::Vector2f{ 100.0f, 50.0f })),
              m_ExitButtonCollision(CreateShared<RectCollision>(sf::Vector2f{ windowSize.x / 2.0f - 50.0f, windowSize.y / 2.0f }, sf::Vector2f{ 100.0f, 50.0f })),
              m_WindowSize(windowSize)
        {
            m_StartSimButton.SetCollisionChecker(m_StartButtonCollision);
            m_ExitButton.SetCollisionChecker(m_ExitButtonCollision);

            m_StartSimButton.SetOnClickCallback(
                [this](MouseButtonPressed& e) {
                    if (e.Data.button == sf::Mouse::Button::Left)
                        m_SimSetting = true;
                    return true;
                });
            m_ExitButton.SetOnClickCallback(
                [](MouseButtonPressed& e) {
                    if (e.Data.button == sf::Mouse::Button::Left)
                        UserEventHandler::Get().SendEvent(CloseApp{});
                    return true;
                });

            if (!m_StartTexture.loadFromFile(SPRITES_FOLDER "start_button.png"))
                CW_ERROR("Unable to load start button texture!");
            if (!m_ExitTexture.loadFromFile(SPRITES_FOLDER "exit_button.png"))
                CW_ERROR("Unable to load exit button texture!");
            if (!m_BackgroundShader.loadFromFile(SHADERS_FOLDER "main_menu_fragment.glsl", sf::Shader::Type::Fragment))
                CW_ERROR("Unable to load main menu shader!");
            m_BackgroundShader.setUniform("iResolution", m_WindowSize);
        }

        void Update(float deltaTime) override
        {
            m_ElapsedTime += deltaTime;
            if (m_SimSetting)
            {
                ImGui::SetNextWindowSize({ 300.0f, 100.0f }, ImGuiCond_Appearing);
                ImGui::SetNextWindowPos({ m_WindowSize.x / 2.0f - 150.0f, m_WindowSize.y / 2.0f - 200.0f }, ImGuiCond_Appearing);
                if (ImGui::Begin("Simulation settings", &m_SimSetting, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
                {
                    if (ImGui::Button("Start simulation"))
                    {
                        UserEventHandler::Get().SendEvent(StartSimulation{});
                        m_UpdateActive = false;
                        m_DrawActive = false;
                        m_SimStarted = true;
                        m_SimSetting = false;
                    }
                    ImGui::End();
                }
            }
        }

        void Draw() override
        {
            auto& renderer = Renderer::Get();
            // background
            m_BackgroundShader.setUniform("iTime", m_ElapsedTime);
            renderer.ApplyDefaultView();
            renderer.BeginRectangleShape()
                .DefaultAfterDraw()
                .Size(m_WindowSize)
                .Shader(&m_BackgroundShader)
                .Draw();
            // Start sim
            renderer.BeginRectangleShape()
                .Size(m_StartButtonCollision->GetSize())
                .Position(m_StartButtonCollision->GetPos())
                .Texture(&m_StartTexture)
                .OutlineThickness(2.0f)
                .OutlineColor({ 25, 240, 25 })
                .Draw();
            // Exit
            renderer.BeginRectangleShape()
                .Size(m_ExitButtonCollision->GetSize())
                .Position(m_ExitButtonCollision->GetPos())
                .Texture(&m_ExitTexture)
                .OutlineThickness(2.0f)
                .OutlineColor({ 240, 25, 25 })
                .Draw()
                .SetDefault();
        }

        void OnEvent(Event& event) override
        {
            EventDispatcher dispatcher(event);
            if (dispatcher.Dispach<WindowResized>(CW_BUILD_EVENT_FUNC(OnWindowResized)))
                return;
            if (!m_SimStarted)
            {
                m_StartSimButton.OnEvent(event);
                m_ExitButton.OnEvent(event);
            }
        }

    private:
        bool OnWindowResized(WindowResized& e)
        {
            m_WindowSize = static_cast<sf::Vector2f>(e.Size);
            m_StartButtonCollision->SetRect(sf::FloatRect({ m_WindowSize.x / 2.0f - 50.0f, m_WindowSize.y / 2.0f - 100.0f }, { 100.0f, 50.0f }));
            m_ExitButtonCollision->SetRect(sf::FloatRect({ m_WindowSize.x / 2.0f - 50.0f, m_WindowSize.y / 2.0f }, { 100.0f, 50.0f }));
            m_BackgroundShader.setUniform("iResolution", m_WindowSize);
            return false;
        }

    private:
        sf::Texture m_StartTexture;
        sf::Texture m_ExitTexture;
        sf::Vector2f m_WindowSize;
        float m_ElapsedTime = 0.0f;
        sf::Shader m_BackgroundShader;

        Shared<RectCollision> m_StartButtonCollision;
        Shared<RectCollision> m_ExitButtonCollision;
        Button m_StartSimButton;
        Button m_ExitButton;

        bool m_SimSetting = false;
        bool m_SimStarted = false;
    };


    class MyApp
        : public Application
    {
    public:
        MyApp()
            : Application(800, 600, "test")
        {
            //m_ClearColor = sf::Color(84, 87, 91, 255);
            m_ClearColor = sf::Color(0, 0, 0, 255);
        }

        void Init() override
        {
            PushLayer<MainMenuLayer>(static_cast<sf::Vector2f>(GetWindowSize()));
        }

        void Update(float deltaTime) override
        {
            CW_PROFILE_FUNCTION();
            UpdateInterface();
            UpdateLayers(deltaTime);
        }

        void PauseUpdate(float deltaTime) override
        {
        }

        void Draw() override
        {
            CW_PROFILE_FUNCTION();
            DrawLayers();
        }

        void OnEvent(Event& event) override
        {
            EventDispatcher dispatcher(event);
            if (dispatcher.Dispach<CloseApp>(CW_BUILD_EVENT_FUNC(OnCloseApp)))
                return;
            if (dispatcher.Dispach<StartSimulation>(CW_BUILD_EVENT_FUNC(OnStartSimulation)))
                return;
            dispatcher.Dispach<WindowResized>(CW_BUILD_EVENT_FUNC(OnWindowResized));
            OnEventLayers(event);
        }

    private:
        bool OnKeyPressed(KeyPressed& event)
        {
            if (event.Data.code == sf::Keyboard::Key::Space)
            {
                SwitchPause();
            }
            return false;
        }

        bool OnWindowResized(WindowResized& e)
        {
            m_WindowSize = e.Size;
            return false;
        }

        bool OnCloseApp(CloseApp&)
        {
            Close();
            return true;
        }

        bool OnStartSimulation(StartSimulation&)
        {
            InsertLayer<SimulationLayer>(0, static_cast<sf::Vector2f>(GetWindowSize()));
            m_ClearColor = sf::Color(135, 206, 235, 255);
            return true;
        }

        void UpdateInterface()
        {
            ImGui::Begin("Debug");
            if (ImGui::CollapsingHeader("App-statistics"))
            {
                const auto& render = Renderer::Get().GetRenderTarget();
                sf::Vector2i mousePos = sf::Mouse::getPosition();
                ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("paused: %d", IsPaused());
                ImGui::Text("mouse position: (%d, %d)", mousePos.x, mousePos.y);
                ImGui::Text("world mouse position: (%.2f, %.2f)",
                    render.mapPixelToCoords(mousePos).x,
                    render.mapPixelToCoords(mousePos).y);
                ImGui::Text("render target view size: (%.2f, %.2f)", render.getView().getSize().x, render.getView().getSize().y);
                ImGui::Text("render target view center: (%.2f, %.2f)", render.getView().getCenter().x, render.getView().getCenter().y);
                ImGui::Spacing();
            }
            ImGui::End();
        }
    };

} // CW

std::unique_ptr<CW::Application> create_program(int argc, const char** argv)
{
    return std::make_unique<CW::MyApp>();
}
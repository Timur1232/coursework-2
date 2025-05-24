#include "pch.h"

#include "engine/EntryPoint.h"
#include "engine/ProgramCore.h"
#include "engine/Events/CoreEvents.h"
#include "engine/Events/UserEvents.h"
#include "engine/Renderer.h"
#include "engine/Events/UserEventHandler.h"
#include "engine/Button.h"
#include "engine/UPSLimiter.h"

#include "debug_utils/Log.h"
#include "debug_utils/Profiler.h"
#include "utils/ArenaAllocator.h"
#include "ObjectPallete.h"

#include "SimState.h"
#include "Camera2D.h"
#include "Beacon.h"
#include "Drone.h"
#include "BitDirection.h"
#include "Terrain.h"
#include "SimulationSettings.h"

#define SHADERS_FOLDER "res/shaders/"
#define SPRITES_FOLDER "res/sprites/"

namespace CW {

    char g_FilePathBuff[512] = "";


    class SimulationLayer
        : public Layer
    {
    public:
        SimulationLayer(sf::Vector2f windowSize, const SimulationSettings& settings, FullSimulationState* state = nullptr)
            : m_Camera(0, 0, windowSize.x, windowSize.y),
              m_Settings(settings),
              m_Drones(settings.Drones),
              m_Beacons(settings.Beacons),
              m_Resources(settings.Resources),
              m_Terrain(settings.TerrainGenerator),
              m_ResourceReciever(settings.Drones)
        {
            m_DroneTexture = CreateUnique<sf::Texture>("res/sprites/drone_sprite.png");
            m_DroneSprite = CreateUnique<sf::Sprite>(*m_DroneTexture);
            m_DroneSprite->setOrigin(static_cast<sf::Vector2f>(m_DroneTexture->getSize()) / 2.0f);
            if (state)
            {
                m_Settings = state->Settings;
                m_Drones.SetState(*state);
                m_Beacons.SetState(*state);
                m_Resources.SetState(*state);
                m_ResourceReciever.SetData(state->RecieverData);
                m_Terrain.SetSettings(state->Settings.TerrainGenerator);
                m_GeneratedRange = state->GeneratedRange;
            }
            Init(windowSize);
            if (!state)
                InitNewSim(settings.DronesCount, { settings.StartingHorizontalPosition, 0.0f });
        }

        ~SimulationLayer()
        {
            if (!m_RunSingleThread)
                SyncStopSimulationThread();
        }

        void SpawnSimulationThread()
        {
            m_SimulationThread = std::thread(
                [this] {
                    this->SimulationCycle();
                });
        }

        void SyncStopSimulationThread()
        {
            m_SimulationStopRequest = true;
            m_SimulationThread.join();
            m_SimulationStopRequest = false;
        }

        void Init(const sf::Vector2f& windowSize)
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
            GenerateMeshes(m_Terrain.GetTerrain(), m_GeneratedRange);
            m_LeftGeneratedSections.x = m_LeftGeneratedSections.y = m_GeneratedRange.x;
            m_RightGeneratedSections.x = m_RightGeneratedSections.y = m_GeneratedRange.y;

            m_WaterShader.setUniform("uResolution", windowSize);
            m_WaterShader.setUniform("uDeepDarkFactor", 3.5f);
            m_WaterShader.setUniform("uWaterYOffset", 300.0f);

            m_DarkeningShader.setUniform("uResolution", windowSize);
            m_DarkeningShader.setUniform("uDarkeningFactor", 25.0f);
            m_DarkeningShader.setUniform("uYOffset", 3000.0f);
        }

        void Update(float deltaTime) override
        {
            if (m_RunSingleThread)
                OneIteration(deltaTime);
        }

        void OnEvent(Event& event) override
        {
            EventDispatcher dispatcher(event);
            /*if (dispatcher.Dispach<CreateBeacon>(CW_BUILD_EVENT_FUNC(OnCreateBeacon)))
                return;*/
            if (dispatcher.Dispach<SetSimulationSettings>(CW_BUILD_EVENT_FUNC(OnSimSettings)))
                return;
            if (dispatcher.Dispach<SaveSimulation>(CW_BUILD_EVENT_FUNC(OnSaveSimulation)))
                return;
            if (dispatcher.Dispach<SwitchThread>(CW_BUILD_EVENT_FUNC(OnSwitchThread)))
                return;
            dispatcher.Dispach<MouseButtonPressed>(CW_BUILD_EVENT_FUNC(OnMouseButtonPressed));
            dispatcher.Dispach<MouseButtonReleased>(CW_BUILD_EVENT_FUNC(OnMouseButtonReleased));
            dispatcher.Dispach<WindowResized>(CW_BUILD_EVENT_FUNC(OnWindowResized));

            m_Camera.OnEvent(event);
            //m_Drones.OnEvent(event);
        }

        void Draw() override
        {
            return;
            m_RenderState.Clear();

            // синхронизация для копирования состояния
            if (!m_RunSingleThread)
                SyncStopSimulationThread();

            GenerateChunksInCameraView();

            CollectState(m_RenderState);
            sf::Vector2i leftGen = m_LeftGeneratedSections;
            sf::Vector2i rightGen = m_RightGeneratedSections;
            m_LeftGeneratedSections.y = m_LeftGeneratedSections.x;
            m_RightGeneratedSections.x = m_RightGeneratedSections.y;

            float elapsedTime = m_ElapsedTime;

            if (!m_RunSingleThread)
                SpawnSimulationThread();


            m_WaterShader.setUniform("uCameraPosition", m_Camera.GetView().getCenter());
            m_WaterShader.setUniform("uZoomFactor", m_Camera.GetZoomFactor());
            m_WaterShader.setUniform("uTime", elapsedTime);

            m_DarkeningShader.setUniform("uZoomFactor", m_Camera.GetZoomFactor());
            m_DarkeningShader.setUniform("uCameraPosition", m_Camera.GetView().getCenter());
            m_DarkeningShader.setUniform("uCameraViewYSize", m_Camera.GetView().getSize().y);

            auto& renderer = Renderer::Get();
            renderer.ApplyDefaultView();
            renderer.BeginRectangleShape()
                .DefaultAfterDraw()
                .Size(renderer.GetWindowSize())
                .Shader(&m_WaterShader)
                .Draw();
            renderer.SetView(m_Camera.GetView());

            /*if (m_DrawChunks)
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
            }*/

            

            //m_ResourceReciever.Draw();
            auto& circleBuilder = Renderer::Get().BeginCircleShape();
            // body
            circleBuilder.DefaultAfterDraw()
                .Radius(100.0f)
                .Position(m_RenderState.ResieverPosition)
                .Color(sf::Color::Magenta)
                .Draw();
            // recieve radius
            //circleBuilder.DefaultAfterDraw()
            //    .Radius(m_RenderState.Re)
            //    .Position(m_RenderState.ResieverPosition)
            //    .Color(sf::Color::Transparent)
            //    .OutlineThickness(1.0f)
            //    .OutlineColor(sf::Color::Green)
            //    .Draw();
            //// broadcast radius
            //circleBuilder.DefaultAfterDraw()
            //    .Radius(m_BroadcastRadius)
            //    .Position(m_Position)
            //    .Color(sf::Color::Transparent)
            //    .OutlineThickness(1.0f)
            //    .OutlineColor(sf::Color::Red)
            //    .Draw();

            //m_Resources.DrawAllRecources();
            for (size_t i = 0; i < m_RenderState.ResourcesPositions.size(); ++i)
            {
                Renderer::Get().BeginCircleShape()
                    .DefaultAfterDraw()
                    .Radius(20.0f)
                    .Color(sf::Color::Cyan)
                    .Position(m_RenderState.ResourcesPositions[i])
                    .Draw();
            }

            if (m_DrawBeacons)
            {
                CW_PROFILE_SCOPE("beacons draw");
                //m_Beacons.DrawAllBeacons();
                auto& circleBuilder = Renderer::Get().BeginCircleShape();
                circleBuilder.PointCount(4)
                    .Radius(10.0f);
                for (size_t i = 0; i < m_RenderState.BeaconsPositions.size(); ++i)
                {
                    circleBuilder.Position(m_RenderState.BeaconsPositions[i])
                        .Color(beacon_color(m_RenderState.BeaconsTypes[i], m_RenderState.BeaconsCharges[i]))
                        .Draw();
                }
                circleBuilder.SetDefault();
            }

            {
                CW_PROFILE_SCOPE("drones draw");
                //m_Drones.DrawAllDrones();
                for (size_t i = 0; i < m_RenderState.DronesDirections.size(); ++i)
                {
                    /*debugDrawDirectionVisuals(drone.GetPos(), drone.GetDirection(), drone.GetAttraction());
                    debugDrawViewDistance(drone.GetPos(), drone.GetDirection());*/

                    m_DroneSprite->setPosition(m_RenderState.DronesPositions[i]);
                    if (abs(m_RenderState.DronesDirections[i].asRadians()) > angle::PI_2)
                    {
                        m_DroneSprite->setScale({ 1.0f, -1.0f });
                    }
                    else
                    {
                        m_DroneSprite->setScale({ 1.0f, 1.0f });
                    }
                    m_DroneSprite->setRotation(m_RenderState.DronesDirections[i]);
                    Renderer::Get().Draw(*m_DroneSprite);
                }
            }
          
            {
                CW_PROFILE_SCOPE("terrain draw");
                // генерация нового меша
                GenerateMeshes(m_RenderState.Terrain, leftGen);
                GenerateMeshes(m_RenderState.Terrain, rightGen);
                
                for (const auto& mesh : m_TerrainSectionMeshes)
                {
                    renderer.Draw(mesh);
                    renderer.Draw(mesh, &m_DarkeningShader);
                }
            }
        }

        void CollectState(SimulationState& state)
        {
            m_Drones.CollectState(state);
            m_Beacons.CollectState(state);
            m_Resources.CollectState(state);
            state.ResieverPosition = m_ResourceReciever.GetPos();
            state.ResourceCount = m_ResourceReciever.GetResources();
            state.Terrain = m_Terrain.GetTerrain();
        }

        void CollectState(FullSimulationState& state)
        {
            m_Drones.CollectState(state);
            m_Beacons.CollectState(state);
            m_Resources.CollectState(state);
            state.GeneratedRange = m_GeneratedRange;
            state.RecieverData = m_ResourceReciever.GetData();
            state.Settings = m_Settings;
        }

    private:
        void OneIteration(float deltaTime)
        {
            GenerateChunksForDrones();

            if (m_ResourceReciever.Update(deltaTime))
            {
                // создание дрона
                sf::Angle randAngle = sf::degrees(lerp(0.0f, 180.0f, rand_float()));
                m_Drones.CreateDrone(m_ResourceReciever.GetPos(), randAngle);
                CW_TRACE("Drone spawned: count {}", m_Drones.Size());
            }

            {
                CW_PROFILE_SCOPE("beacons update");
                m_Beacons.Update(deltaTime);
            }
            {
                CW_PROFILE_SCOPE("drones update");
                std::vector<BeaconComponents> componentsVec = m_Drones.UpdateAllDrones(deltaTime, m_Resources.GetResources(),
                    m_Beacons.GetChuncks(), m_ResourceReciever, m_Terrain);
                for (auto [pos, type, bitDir] : componentsVec)
                {
                    // создание маяков
                    m_Beacons.CreateBeacon(pos, type, bitDir);
                }
            }
            m_ElapsedTime += deltaTime;
        }

        void SimulationCycle()
        {
            while (!m_SimulationStopRequest)
            {
                OneIteration(c_FixedDeltaTime);
                m_Limiter.Wait();
            }
        }

        bool OnMouseButtonPressed(MouseButtonPressed& e)
        {
            /*if (!m_Hold && !ImGui::GetIO().WantCaptureMouse && e.Data.button == sf::Mouse::Button::Left)
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
            }*/
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
            return false;
        }

        /*bool OnCreateBeacon(CreateBeacon& e)
        {
            m_Beacons.CreateBeacon(e.Position, e.Type, e.BitDirection);
            return false;
        }*/

        bool OnSimSettings(SetSimulationSettings& e)
        {
            m_Settings = *e.Settings;
            m_Beacons.SetSettings(e.Settings->Beacons);
            m_Drones.SetSettings(e.Settings->Drones);
            m_Resources.SetSettings(e.Settings->Resources);
            m_Terrain.SetSettings(e.Settings->TerrainGenerator);
            return true;
        }

        void InitNewSim(size_t droneCount, sf::Vector2f startPosition)
        {
            m_Beacons.Clear();
            m_Drones.Reset(droneCount, startPosition, TargetType::Recource);
            CW_TRACE("Starting simulation with {} drones", droneCount);
        }

        void RestartSim(size_t droneCount, sf::Vector2f startPosition = { 0.0f, 0.0f }, TargetType target = TargetType::Recource)
        {
            m_Beacons.Clear();
            m_Drones.Reset(droneCount, startPosition, target);
            m_Resources.Clear();
            CW_TRACE("Restarting simulation with {} drones", droneCount);
        }
        
        bool OnSaveSimulation(SaveSimulation& e)
        {
            std::ofstream file(e.FilePath, std::ios::binary);
            if (!file)
            {
                CW_ERROR("Unable to open file for saving!");
                return true;
            }

            FullSimulationState state;
            SyncStopSimulationThread();
            CollectState(state);
            SpawnSimulationThread();

            // reciever
            file.write(reinterpret_cast<const char*>(&state.RecieverData), sizeof(state.RecieverData));

            // terrain
            file.write(reinterpret_cast<const char*>(&state.GeneratedRange), sizeof(state.GeneratedRange));

            // settings
            file.write(reinterpret_cast<const char*>(&state.Settings), sizeof(state.Settings));

            // drones
            size_t size = state.Drones.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
            for (const auto& drone : state.Drones)
            {
                drone.WriteToFile(file);
            }

            // beacons
            size = state.Beacons.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
            for (const auto& beacon : state.Beacons)
            {
                beacon.WriteToFile(file);
            }

            // resources
            size = state.Resources.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
            for (const auto& resource : state.Resources)
            {
                resource.WriteToFile(file);
            }

            return true;
        }

        bool OnSwitchThread(SwitchThread& e)
        {
            if (m_RunSingleThread)
            {
                m_RunSingleThread = false;
                m_Limiter.SetUPS(e.TargetUps);
                m_Limiter.Reset();
                SpawnSimulationThread();
            }
            else
            {
                m_RunSingleThread = true;
                SyncStopSimulationThread();
            }
            return true;
        }

        /*void UpdateInterface() 
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
                    static int seed = m_Terrain.GetDetailedSeed();
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
        }*/

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
                m_LeftGeneratedSections.x = leftBorderKey;
                m_GeneratedRange.x = leftBorderKey;
            }
            if (rightBorderKey >= m_GeneratedRange.y && cameraViewRect.contains({ rightBorderX, cameraViewRect.getCenter().y }))
            {
                rightBorderKey += 1;
                GenerateChunkRange(m_GeneratedRange.y, rightBorderKey);
                m_RightGeneratedSections.y = rightBorderKey;
                m_GeneratedRange.y = rightBorderKey;
            }
        }

        void GenerateChunkRange(int left, int right)
        {
            for (int key = left; key < right; ++key)
            {
                if (!m_Terrain.Generate(key))
                    continue;
                //GenerateMesh(key);
                m_Resources.GenerateResourceOnSection(m_Terrain, key);
            }
        }

        void GenerateMeshes(const Terrain& terrain, sf::Vector2i range)
        {
            while (range.x < range.y)
            {
                auto& mesh = m_TerrainSectionMeshes.emplace_back();
                generate_mesh(terrain, mesh, range.x);
                mesh.setTexture(&m_TerrainTexture);
                mesh.setTextureRect({ { 0, 0 }, static_cast<sf::Vector2i>(mesh.getLocalBounds().size) });
                range.x += 1;
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
                m_LeftGeneratedSections.x = leftBorderKey;
                m_GeneratedRange.x = leftBorderKey;
            }
            if (rightBorderKey >= m_GeneratedRange.y)
            {
                rightBorderKey += 1;
                GenerateChunkRange(m_GeneratedRange.y, rightBorderKey);
                m_RightGeneratedSections.y = rightBorderKey;
                m_GeneratedRange.y = rightBorderKey;
            }

            //for (int key = genRangeLeft; key < genRangeRight; ++key)
            //{
            //    if (!m_Terrain.Generate(key))
            //        continue;
            //    //GenerateMesh(key);
            //    m_Resources.GenerateResourceOnSection(m_Terrain, key);
            //}
        }

    private:
        std::thread m_SimulationThread;
        const float c_FixedDeltaTime = 1.0f / 60.0f;
        std::atomic<bool> m_SimulationStopRequest = false;

        UPSLimiter m_Limiter{ 120 };
        bool m_RunSingleThread = true;

        SimulationState m_RenderState;

        Camera2D m_Camera;
        bool m_Hold = false;

        SimulationSettings m_Settings;

        BeaconManager m_Beacons;
        DroneManager m_Drones;
        ResourceManager m_Resources;
        ResourceReciever m_ResourceReciever;

        TerrainGenerator m_Terrain;
        sf::Vector2i m_GeneratedRange{-5, 5};
        sf::Vector2i m_LeftGeneratedSections{ 0, 0 };
        sf::Vector2i m_RightGeneratedSections{ 0, 0 };

        std::vector<sf::ConvexShape> m_TerrainSectionMeshes;
        sf::Texture m_TerrainTexture;

        float m_ElapsedTime = 0.0f;

        sf::Shader m_WaterShader;
        sf::Shader m_DarkeningShader;

        Unique<sf::Sprite> m_DroneSprite;
        Unique<sf::Texture> m_DroneTexture;

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
        enum class MenuStatus
        {
            None = 0,
            SimSetting,
            LoadSim
        };

    public:
        MainMenuLayer(sf::Vector2f windowSize)
            : m_StartButtonCollision(CreateShared<RectCollision>(sf::Vector2f{ windowSize.x / 2.0f - 50.0f, windowSize.y / 2.0f - 100.0f }, sf::Vector2f{ 100.0f, 50.0f })),
              m_LoadButtonCollision(CreateShared<RectCollision>(sf::Vector2f{ windowSize.x / 2.0f - 50.0f, windowSize.y / 2.0f }, sf::Vector2f{ 100.0f, 50.0f })),
              m_ExitButtonCollision(CreateShared<RectCollision>(sf::Vector2f{ windowSize.x / 2.0f - 50.0f, windowSize.y / 2.0f + 100.0f }, sf::Vector2f{ 100.0f, 50.0f }))
        {
            m_StartSimButton.SetCollisionChecker(m_StartButtonCollision);
            m_LoadButton.SetCollisionChecker(m_LoadButtonCollision);
            m_ExitButton.SetCollisionChecker(m_ExitButtonCollision);

            m_StartSimButton.SetOnClickCallback(
                [this](MouseButtonPressed& e) {
                    if (e.Data.button == sf::Mouse::Button::Left)
                        m_MenuStatus = MenuStatus::SimSetting;
                    return true;
                });
            m_LoadButton.SetOnClickCallback(
                [this](MouseButtonPressed& e) {
                    if (e.Data.button == sf::Mouse::Button::Left)
                        m_MenuStatus = MenuStatus::LoadSim;
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
            if (!m_LoadTexture.loadFromFile(SPRITES_FOLDER "load_button.png"))
                CW_ERROR("Unable to load load button texture!");
            if (!m_BackgroundShader.loadFromFile(SHADERS_FOLDER "main_menu_fragment.glsl", sf::Shader::Type::Fragment))
                CW_ERROR("Unable to load main menu shader!");
            m_BackgroundShader.setUniform("iResolution", windowSize);
        }

        void Update(float deltaTime) override
        {
            m_ElapsedTime += deltaTime;
            switch (m_MenuStatus)
            {
            case MenuStatus::SimSetting: SimulationSettingsMenu(); break;
            case MenuStatus::LoadSim:    LoadSimulationInterface(); break;
            default: break;
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
                .Size(renderer.GetWindowSize())
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
            // Load
            renderer.BeginRectangleShape()
            .Size(m_LoadButtonCollision->GetSize())
                .Position(m_LoadButtonCollision->GetPos())
                .Texture(&m_LoadTexture)
                .OutlineThickness(2.0f)
                .OutlineColor({ 14, 77, 148 })
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
            m_StartSimButton.OnEvent(event);
            m_LoadButton.OnEvent(event);
            m_ExitButton.OnEvent(event);
        }

    private:
        bool OnWindowResized(WindowResized& e)
        {
            m_StartButtonCollision->SetRect(sf::FloatRect({ e.Size.x / 2.0f - 50.0f, e.Size.y / 2.0f - 100.0f }, { 100.0f, 50.0f }));
            m_LoadButtonCollision->SetRect(sf::FloatRect({ e.Size.x / 2.0f - 50.0f, e.Size.y / 2.0f }, { 100.0f, 50.0f }));
            m_ExitButtonCollision->SetRect(sf::FloatRect({ e.Size.x / 2.0f - 50.0f, e.Size.y / 2.0f + 100.0f }, { 100.0f, 50.0f }));
            m_BackgroundShader.setUniform("iResolution", (sf::Vector2f) e.Size);
            return false;
        }

        void SimulationSettingsMenu()
        {
            sf::Vector2f windowSize = Renderer::Get().GetWindowSize();
            ImGui::SetNextWindowSize({ 550.0f, 500.0f }, ImGuiCond_Appearing);
            ImGui::SetNextWindowPos({ windowSize.x / 2.0f - 275.0f, windowSize.y / 2.0f - 250.0f}, ImGuiCond_Appearing);
            bool simSetWind = (m_MenuStatus == MenuStatus::SimSetting) ? true : false;
            if (ImGui::Begin("Simulation settings", &simSetWind, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
            {
                /*auto size = ImGui::GetWindowSize();
                ImGui::Text("window size: (%.2f, %.2f)", size.x, size.y);*/
                float height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
                ImGui::BeginChild("SturtupSettings", ImVec2(ImGui::GetContentRegionAvail().x, -height));
                
                DroneSetupInterface();
                BeaconSetupInterface();
                TerrainSetupInterface();

                ImGui::EndChild();

                ImGui::Separator();
                if (ImGui::Button("Start simulation"))
                {
                    UserEventHandler::Get().SendEvent(StartSimulation{&m_SimSettings});
                }
                ImGui::End();
            }
            if (!simSetWind)
                m_MenuStatus = MenuStatus::None;
        }

        void TerrainSetupInterface()
        {
            if (ImGui::CollapsingHeader("Terrain"))
            {
                ImGui::Separator();
                auto& terrain = m_SimSettings.TerrainGenerator;
                ImGui::InputFloat("Y offset (deepness)", &terrain.YOffset);
                ImGui::InputInt("Samples per section", &terrain.SamplesPerSection);
                ImGui::InputFloat("Section width", &terrain.SectionWidth);
                ImGui::InputFloat("Max height", &terrain.MaxHeight, 10.0f, 100.0f);

                if (ImGui::TreeNode("Noise##settings"))
                {
                    ImGui::SeparatorText("\"Bell\" function");
                    ImGui::InputFloat("Bell height", &terrain.BellHeigth, 10.0f, 100.0f);
                    ImGui::InputFloat("Bell width", &terrain.BellWidth, 10.0f, 100.0f);

                    ImGui::SeparatorText("Noise generator");
                    ImGui::InputInt("Base seed", &terrain.BaseSeed, 1, 10);
                    ImGui::InputInt("Detailed seed", &terrain.DetailedSeed, 1, 10);
                    ImGui::InputFloat("Base frequensy", &terrain.BaseFrequensy, 0.01f, 0.1f);
                    ImGui::InputFloat("Detailed frequensy", &terrain.DetailedFrequensy, 0.01f, 0.1f);
                    ImGui::InputFloat("Base factor", &terrain.BaseFactor, 0.01f, 0.1f);
                    ImGui::InputFloat("Detailed factor", &terrain.DetailedFactor, 0.1f, 0.2f);
                    ImGui::InputFloat("Mapped noise distance", &terrain.MappedNoiseDistance, 0.1f, 0.2f);

                    ImGui::Spacing();

                    ImGui::InputFloat("Gain", &terrain.Gain);
                    ImGui::InputFloat("Weighted strength", &terrain.WeightedStrength);
                    ImGui::InputInt("Octaves", &terrain.Octaves);
                    ImGui::InputFloat("Lacunarity", &terrain.Lacunarity);
                    ImGui::TreePop();
                }
                if (ImGui::Button("set default##terrain"))
                    terrain.SetDefault();
                ImGui::Separator();
            }
        }

        void BeaconSetupInterface()
        {
            if (ImGui::CollapsingHeader("Beacons"))
            {
                ImGui::Separator();
                auto& beacons = m_SimSettings.Beacons;
                ImGui::InputFloat("Discharge rate", &beacons.DischargeRate, 0.1f, 0.2f);
                ImGui::InputFloat("Charge threshold", &beacons.ChargeThreshold, 0.01f, 0.1f);

                if (ImGui::Button("set default##beacon"))
                    beacons.SetDefault();
                ImGui::Separator();
            }
        }

        void DroneSetupInterface()
        {
            if (ImGui::CollapsingHeader("Drones"))
            {
                auto& drones = m_SimSettings.Drones;
                ImGui::Separator();
                ImGui::InputInt("Drones count", &m_SimSettings.DronesCount);
                ImGui::InputFloat("Starting position", &m_SimSettings.StartingHorizontalPosition);
                ImGui::Spacing();

                ImGui::InputFloat("Drone spawn cooldown", &drones.BeaconCooldownSec, 1.0f, 10.0f);
                ImGui::Spacing();
                ImGui::InputFloat("Speed", &drones.Speed, 10.0f, 100.0f);
                ImGui::SliderAngle("Turning speed", &drones.TurningSpeed, 1.0f, 180.0f);
                ImGui::InputFloat("FOV", &drones.FOV, 0.01f, 0.1f);
                ImGui::InputFloat2("View distance", &drones.ViewDistance.x);
                ImGui::InputFloat("Pickup distance", &drones.PickupDist, 1.0f, 10.0f);
                ImGui::Spacing();
                ImGui::InputFloat("Beacon spawn cooldown", &drones.BeaconCooldownSec, 0.5f, 1.0f);
                ImGui::InputFloat("Wander cooldown", &drones.BeaconCooldownSec, 0.5f, 1.0f);
                ImGui::Spacing();
                ImGui::SliderAngle("Random wander angle", &drones.RandomWanderAngleRad, 1.0f, 180.0f);
                ImGui::SliderFloat("Wander angle threashold", &drones.WanderAngleThresholdDeg, 0.0f, 15.0f);
                ImGui::SliderAngle("Max turning delta", &drones.MaxTurningDeltaRad, 1.0f, 180.0f);

                if (ImGui::Button("set default##drone"))
                {
                    drones.SetDefault();
                }
                ImGui::Separator();
            }
        }

        void LoadSimulationInterface()
        {
            bool simLoadWind = (m_MenuStatus == MenuStatus::LoadSim) ? true : false;
            if (ImGui::Begin("Load save", &simLoadWind))
            {
                ImGui::InputText("Path", g_FilePathBuff, sizeof(g_FilePathBuff) / sizeof(*g_FilePathBuff));
                if (ImGui::Button("Load"))
                {
                    UserEventHandler::Get().SendEvent(LoadSimulation{ g_FilePathBuff });
                }
                ImGui::End();
            }
            if (!simLoadWind)
                m_MenuStatus = MenuStatus::None;
        }

    private:
        sf::Texture m_StartTexture;
        sf::Texture m_LoadTexture;
        sf::Texture m_ExitTexture;
        float m_ElapsedTime = 0.0f;
        sf::Shader m_BackgroundShader;

        Shared<RectCollision> m_StartButtonCollision;
        Shared<RectCollision> m_ExitButtonCollision;
        Shared<RectCollision> m_LoadButtonCollision;
        Button m_StartSimButton;
        Button m_LoadButton;
        Button m_ExitButton;

        MenuStatus m_MenuStatus = MenuStatus::None;

        SimulationSettings m_SimSettings;
    };


    class InterfaceLayer
        : public Layer
    {
    public:
        InterfaceLayer(sf::Vector2f windowSize)
            : m_MenuButtonCollision(CreateShared<CircleCollision>(sf::Vector2f{windowSize.x - 35.0f, 35.0f}, 25.0f)),
              m_DebugMenuButtonCollision(CreateShared<CircleCollision>(sf::Vector2f{ windowSize.x - 35.0f, 95.0f }, 25.0f))
        {
            m_MenuButton.SetCollisionChecker(m_MenuButtonCollision);
            m_DebugMenuButton.SetCollisionChecker(m_DebugMenuButtonCollision);

            m_MenuButton.SetOnClickCallback(
                [](MouseButtonPressed&) {
                    UserEventHandler::Get().SendEvent(SwitchMenu{});
                    return true;
                });
            m_DebugMenuButton.SetOnClickCallback(
                [](MouseButtonPressed&) {
                    UserEventHandler::Get().SendEvent(SwitchDebugMenu{});
                    return true;
                });

            m_UpdateActive = false;
        }

        void Update(float) override {}

        void Draw() override
        {
            auto& renderer = Renderer::Get();
            renderer.ApplyDefaultView();
            // menu button
            renderer.BeginCircleShape()
                .Radius(m_MenuButtonCollision->GetRadius())
                .Position(m_MenuButtonCollision->GetPos())
                .Color({ 220, 220, 220 })
                .Draw();
            // debug menu button
            renderer.BeginCircleShape()
                .Radius(m_DebugMenuButtonCollision->GetRadius())
                .Position(m_DebugMenuButtonCollision->GetPos())
                .Color({ 150, 150, 150 })
                .Draw()
                .SetDefault();
        }
        
        void OnEvent(Event& event) override
        {
            EventDispatcher dispatcher(event);
            if (dispatcher.Dispach<WindowResized>(CW_BUILD_EVENT_FUNC(OnWindowResized)))
                return;
            m_MenuButton.OnEvent(event);
            m_DebugMenuButton.OnEvent(event);
        }

    private:
        bool OnWindowResized(WindowResized& e)
        {
            m_MenuButtonCollision->SetPos({ e.Size.x - 35.0f, 35.0f });
            m_DebugMenuButtonCollision->SetPos({ e.Size.x - 35.0f, 95.0f });
            return false;
        }

    private:
        Button m_MenuButton;
        Button m_DebugMenuButton;
        Shared<CircleCollision> m_MenuButtonCollision;
        Shared<CircleCollision> m_DebugMenuButtonCollision;
    };


    class MyApp
        : public Application
    {
    public:
        MyApp()
            : Application(800, 600, "test")
        {
            m_ClearColor = sf::Color(0, 0, 0, 255);
        }

        void Init() override
        {
            PushLayer<MainMenuLayer>((sf::Vector2f) GetWindowSize());
        }

        void Update(float deltaTime) override
        {
            CW_PROFILE_FUNCTION();
            if (m_ExitFlag)
            {
                PopLayer();
                PopLayer();
                m_ExitFlag = false;
                PushLayer<MainMenuLayer>((sf::Vector2f)GetWindowSize());
                m_ClearColor = sf::Color(0, 0, 0, 255);
                return;
            }

            if (m_DebugIsOpen)
                UpdateDebugInterface();
            if (m_MenuIsOpen)
                UpdateMenuInterface();
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
            if (dispatcher.Dispach<SwitchDebugMenu>(CW_BUILD_EVENT_FUNC(OnSwitchDebugMenu)))
                return;
            if (dispatcher.Dispach<SwitchMenu>(CW_BUILD_EVENT_FUNC(OnSwitchMenu)))
                return;
            if (dispatcher.Dispach<LoadSimulation>(CW_BUILD_EVENT_FUNC(OnLoadSimulation)))
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

        bool OnStartSimulation(StartSimulation& e)
        {
            auto settings = *e.Settings;
            PopLayer();
            PushLayer<SimulationLayer>((sf::Vector2f) GetWindowSize(), settings);
            PushLayer<InterfaceLayer>((sf::Vector2f) GetWindowSize());
            m_ClearColor = sf::Color(135, 206, 235, 255);
            m_DebugIsOpen = false;
            return true;
        }

        bool OnSwitchDebugMenu(SwitchDebugMenu&)
        {
            m_DebugIsOpen = !m_DebugIsOpen;
            return true;
        }

        bool OnSwitchMenu(SwitchMenu&)
        {
            m_MenuIsOpen = !m_MenuIsOpen;
            return true;
        }

        bool OnLoadSimulation(LoadSimulation& e)
        {
            std::ifstream file(e.FilePath, std::ios::binary);
            if (!file)
            {
                CW_ERROR("Unable to open file for loading: {}", e.FilePath);
                return true;
            }

            FullSimulationState state;

            // reciever
            file.read(reinterpret_cast<char*>(&state.RecieverData), sizeof(state.RecieverData));

            // terrain
            file.read(reinterpret_cast<char*>(&state.GeneratedRange), sizeof(state.GeneratedRange));

            // settings
            file.read(reinterpret_cast<char*>(&state.Settings), sizeof(state.Settings));

            size_t size = 0;

            // drones
            file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
            Drone drone;
            for (size_t i = 0; i < size; ++i)
            {
                drone.ReadFromFile(file);
                state.Drones.push_back(drone);
            }

            // beacons
            file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
            Beacon beacon;
            for (size_t i = 0; i < size; ++i)
            {
                beacon.ReadFromFile(file);
                state.Beacons.push_back(beacon);
            }

            // resources
            file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
            Resource resource;
            for (size_t i = 0; i < size; ++i)
            {
                resource.ReadFromFile(file);
                state.Resources.push_back(resource);
            }

            EraseLayer(0);
            PushLayer<SimulationLayer>((sf::Vector2f) GetWindowSize(), state.Settings, &state);
            PushLayer<InterfaceLayer>((sf::Vector2f) GetWindowSize());
            m_ClearColor = sf::Color(135, 206, 235, 255);
            m_DebugIsOpen = false;
            m_CurrentFilePath = e.FilePath;

            CW_TRACE("Simulation loaded successfully: {}", m_CurrentFilePath);

            return true;
        }

        void UpdateDebugInterface()
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
            if (!m_VirtualTime)
            {
                ImGui::SliderInt("Traget UPS", &m_TargetUPS, 60, 1024);
            }
            if (ImGui::Button("Switch virtual time"))
            {
                m_VirtualTime = !m_VirtualTime;
                UserEventHandler::Get().SendEvent(SwitchThread{ m_TargetUPS });
            }
            ImGui::End();
        }

        void UpdateMenuInterface()
        {
            ImGui::Begin("Menu");
            ImGui::InputText("Path", g_FilePathBuff, sizeof(g_FilePathBuff) / sizeof(*g_FilePathBuff));
            if (ImGui::Button("Save"))
            {
                m_CurrentFilePath = g_FilePathBuff;
                UserEventHandler::Get().SendEvent(SaveSimulation{ g_FilePathBuff });
            }
            if (ImGui::Button("Save and Exit"))
            {
                UserEventHandler::Get().SendEvent(SaveSimulation{ m_CurrentFilePath.c_str()});
                m_ExitFlag = true;
                m_MenuIsOpen = false;
                m_DebugIsOpen = false;
            }
            ImGui::End();
        }

    private:
        bool m_MenuIsOpen = false;
        bool m_DebugIsOpen = false;
        bool m_ExitFlag = false;
        bool m_VirtualTime = false;
        int m_TargetUPS = 60;
        std::string m_CurrentFilePath = "";
    };

} // CW

std::unique_ptr<CW::Application> create_program(int argc, const char** argv)
{
    return std::make_unique<CW::MyApp>();
}
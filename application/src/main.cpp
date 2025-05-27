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

#include "SimState.h"
#include "Camera2D.h"
#include "Beacon.h"
#include "Drone.h"
#include "BitDirection.h"
#include "Terrain.h"
#include "SimulationSettings.h"

#define SHADERS_FOLDER "res/shaders/"
#define SPRITES_FOLDER "res/sprites/"
#define SAVES_FOLDER   "saves/"
#define SAVE_EXTENSION ".s"

namespace CW {

    char g_FilePathBuff[512] = "";
    constexpr float DRONE_SCALE_FACTOR = 300.0f / 1024.0f;
    constexpr float RESOURCE_SCALE_FACTOR = 150.0f / 1024.0f;
    constexpr float BEACON_SCALE_FACTOR = 80.0f / 1024.0f;
    constexpr float MOTHER_BASE_SCALE_FACTOR = 600.0f / 1024.0f;
    constexpr ImGuiWindowFlags WINDOW_FLAGS = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    class SimulationLayer
        : public Layer
    {
    public:
        enum class ExitStatus
        {
            None = 0,
            JustSaving,
            SaveAndExit,
            Error,
            Exit
        };
    public:
        SimulationLayer(sf::Vector2f windowSize, Shared<SimulationSettings> settings, FullSimulationState* state = nullptr, const char* savePath = "")
            : m_Camera(0, 0, windowSize.x, windowSize.y),
              m_Settings(settings),
              m_Drones(settings->Drones),
              m_Beacons(settings->Beacons, settings->Drones.ViewDistance.y * 2.0f),
              m_Resources(settings->Resources),
              m_Terrain(settings->TerrainGenerator),
              m_MotherBase(settings->Drones),
              m_CurrentFilePath(savePath)
        {
            // создание спрайта дрона
            m_DroneTexture = CreateUnique<sf::Texture>(SPRITES_FOLDER "drone_sprite.png");
            m_DroneSprite = CreateUnique<sf::Sprite>(*m_DroneTexture);
            m_DroneSprite->setOrigin(static_cast<sf::Vector2f>(m_DroneTexture->getSize()) / 2.0f);
            m_DroneSprite->setScale({ DRONE_SCALE_FACTOR, DRONE_SCALE_FACTOR });

            // создание спрайтов ресурсов
            m_ResourceTextures[0] = CreateUnique<sf::Texture>(SPRITES_FOLDER "scrap_sprite.png");
            m_ResourceTextures[1] = CreateUnique<sf::Texture>(SPRITES_FOLDER "statue_sprite.png");
            m_ResourceTextures[2] = CreateUnique<sf::Texture>(SPRITES_FOLDER "treasure_sprite.png");
            for (size_t i = 0; i < m_ResourceSprites.size(); ++i)
            {
                m_ResourceSprites[i] = CreateUnique<sf::Sprite>(*m_ResourceTextures[i]);
                m_ResourceSprites[i]->setOrigin(static_cast<sf::Vector2f>(m_ResourceTextures[i]->getSize()) / 2.0f);
                m_ResourceSprites[i]->setScale({ RESOURCE_SCALE_FACTOR, RESOURCE_SCALE_FACTOR });
            }

            // создание спрайта базы
            m_MotherBaseTexture = CreateUnique<sf::Texture>(SPRITES_FOLDER "base_sprite.png");
            m_MotherBaseSprite = CreateUnique<sf::Sprite>(*m_MotherBaseTexture);
            m_MotherBaseSprite->setOrigin(static_cast<sf::Vector2f>(m_MotherBaseTexture->getSize()) / 2.0f);
            m_MotherBaseSprite->setScale({ MOTHER_BASE_SCALE_FACTOR, MOTHER_BASE_SCALE_FACTOR });
            sf::Vector2f recieverSpritePos = m_MotherBase.GetPos();
            recieverSpritePos.y = -450.0f;
            m_MotherBaseSprite->setPosition(recieverSpritePos);

            if (state)
            {
                // загрузка состояния из файла
                *m_Settings = state->Settings;
                m_Drones.SetState(*state);
                m_Beacons.SetState(*state);
                m_Resources.SetState(*state);
                m_MotherBase.SetData(state->MotherBaseData);
                m_Terrain.SetSettings(state->Settings.TerrainGenerator);
                m_GeneratedRange = state->GeneratedRange;
            }
            Init(windowSize);
            if (!state)
                // создание чистой симуляции
                InitNewSim(settings->DronesCount, { settings->StartingHorizontalPosition, 0.0f });
            m_SimulationRunning = true;
        }

        ~SimulationLayer()
        {
            if (!m_RunSingleThread)
                SyncStopSimulationThread();
        }

        // запуск потока симуляции
        void SpawnSimulationThread()
        {
            m_SimulationThread = std::thread(
                [this] {
                    this->SimulationCycle();
                });
        }

        // остановка потока с синхронизацией
        void SyncStopSimulationThread()
        {
            m_SimulationStopRequest = true;
            m_SimulationThread.join();
            m_SimulationStopRequest = false;
        }

        // дополнительная инициализация
        void Init(const sf::Vector2f& windowSize)
        {
            m_Resources.Reserve(1024);

            // загрузка ресурсов
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

            // генерация начальной области
            GenerateChunkRange(m_GeneratedRange.x, m_GeneratedRange.y);
            GenerateMeshes(m_Terrain.GetTerrain(), m_GeneratedRange);
            m_LeftGeneratedSections.x = m_LeftGeneratedSections.y = m_GeneratedRange.x;
            m_RightGeneratedSections.x = m_RightGeneratedSections.y = m_GeneratedRange.y;

            // шейдеры
            m_WaterShader.setUniform("uResolution", windowSize);
            m_WaterShader.setUniform("uDeepDarkFactor", 3.5f);
            m_WaterShader.setUniform("uWaterYOffset", 300.0f);

            m_DarkeningShader.setUniform("uResolution", windowSize);
            m_DarkeningShader.setUniform("uDarkeningFactor", 25.0f);
            m_DarkeningShader.setUniform("uYOffset", 3000.0f);
        }

        void Update(float deltaTime) override
        {
            // обновление менюшек
            if (m_DebugIsOpen)
                UpdateDebugInterface();
            if (m_MenuIsOpen)
                UpdateMenuInterface();

            // обновление в одном потоке
            if (m_RunSingleThread && m_SimulationRunning)
                OneIteration(deltaTime);

            // конец симуляции
            if (m_SimulationRunning && m_CopyState.ResourceCount < m_Settings->Drones.DroneCost &&
                (!m_RunSingleThread && m_CopyState.DronesDirections.size() == 0
                || m_RunSingleThread && m_Drones.AliveCount() == 0))
            {
                EventHandler::Get().SendEvent(MessegeToUser{ "All drones are dead and cannot be created! Simulation is over." });
                m_SimulationRunning = false;
                if (!m_RunSingleThread)
                    SyncStopSimulationThread();
                m_RunSingleThread = true;
            }

            if (HandleExit())
                return;
        }

        bool HandleExit()
        {
            switch (m_ExitFlag)
            {
            case ExitStatus::Exit:
            case ExitStatus::SaveAndExit: // корректный выход
                m_SimulationRunning = false;
                if (!m_RunSingleThread)
                    SyncStopSimulationThread();
                m_RunSingleThread = true;
                m_ExitFlag = ExitStatus::None;
                EventHandler::Get().SendEvent(SimulationOver{});
                return true;
            case ExitStatus::Error: // ошибка
                CW_ERROR("Saving error");
                m_ExitFlag = ExitStatus::None;
                break;
            default: m_ExitFlag = ExitStatus::None; break;
            }
            return false;
        }

        // обновление на паузе
        void PausedUpdate(float) override
        {
            if (HandleExit())
                return;
            if (m_DebugIsOpen)
                UpdateDebugInterface();
            if (m_MenuIsOpen)
                UpdateMenuInterface();
        }

        void OnEvent(Event& event) override
        {
            EventDispatcher dispatcher(event);
            if (dispatcher.Dispach<SwitchDebugMenu>(CW_BUILD_EVENT_FUNC(OnSwitchDebugMenu)))
                return;
            if (dispatcher.Dispach<SwitchMenu>(CW_BUILD_EVENT_FUNC(OnSwitchMenu)))
                return;
            dispatcher.Dispach<WindowResized>(CW_BUILD_EVENT_FUNC(OnWindowResized));
            dispatcher.Dispach<KeyPressed>(CW_BUILD_EVENT_FUNC(OnKeyPressed));

            m_Camera.OnEvent(event);
        }

        void Draw() override
        {
            m_CopyState.Clear();

            // синхронизация для копирования состояния
            if (!m_Paused && !m_RunSingleThread)
                SyncStopSimulationThread();

            GenerateChunksInCameraView();

            CollectState(m_CopyState);
            sf::Vector2i leftGen = m_LeftGeneratedSections;
            sf::Vector2i rightGen = m_RightGeneratedSections;
            m_LeftGeneratedSections.y = m_LeftGeneratedSections.x;
            m_RightGeneratedSections.x = m_RightGeneratedSections.y;

            float elapsedTime = m_ElapsedTime;

            if (!m_Paused && !m_RunSingleThread)
                SpawnSimulationThread();

            m_WaterShader.setUniform("uCameraPosition", m_Camera.GetView().getCenter());
            m_WaterShader.setUniform("uZoomFactor", m_Camera.GetZoomFactor());
            m_WaterShader.setUniform("uTime", elapsedTime);

            m_DarkeningShader.setUniform("uZoomFactor", m_Camera.GetZoomFactor());
            m_DarkeningShader.setUniform("uCameraPosition", m_Camera.GetView().getCenter());
            m_DarkeningShader.setUniform("uCameraViewYSize", m_Camera.GetView().getSize().y);

            // отрисовка воды
            auto& renderer = Renderer::Get();
            renderer.ApplyDefaultView();
            renderer.BeginRectangleShape()
                .DefaultAfterDraw()
                .Size(renderer.GetWindowSize())
                .Shader(&m_WaterShader)
                .Draw();
            renderer.SetView(m_Camera.GetView());

            // отрисовка чанков
            if (m_DrawChunks)
            {
                auto& chunkMeshBuilder = renderer.BeginRectangleShape();
                chunkMeshBuilder.Size({ 500.0f, 500.0f })
                    .Color(sf::Color::Transparent)
                    .OutlineThickness(m_Camera.GetZoomFactor())
                    .OutlineColor({ 255, 255, 255, 180 });
                for (auto& chunkPos : m_CopyState.ChunksPositions)
                {
                    chunkMeshBuilder.Position(chunkPos)
                        .Draw();
                }
                chunkMeshBuilder.SetDefault();
            }

            // отрисовка базы
            Renderer::Get().Draw(*m_MotherBaseSprite);

            // отрисовка ресурсов
            for (size_t i = 0; i < m_CopyState.ResourcesPositions.size(); ++i)
            {
                sf::Vector2f resourcePos = m_CopyState.ResourcesPositions[i];
                sf::Angle resourceRot = m_CopyState.ResourcesRotations[i];
                int resourceAmount = m_CopyState.ResourcesAmounts[i];

                size_t spriteIndex = (size_t) (((float) resourceAmount - 5.0f) / (float) m_Settings->Resources.MaxResourceAmount * (float) 3);
                auto& sprite = *m_ResourceSprites[spriteIndex];

                sprite.setPosition(resourcePos);
                sprite.setRotation(resourceRot);
                Renderer::Get().Draw(sprite);
            }

            // отрисовка маяков
            {
                CW_PROFILE_SCOPE("beacons draw");
                auto& circleBuilder = Renderer::Get().BeginCircleShape();
                circleBuilder.PointCount(4)
                    .Radius(10.0f);
                for (size_t i = 0; i < m_CopyState.BeaconsPositions.size(); ++i)
                {
                    circleBuilder.Position(m_CopyState.BeaconsPositions[i])
                        .Color(beacon_color(m_CopyState.BeaconsTypes[i], m_CopyState.BeaconsCharges[i]))
                        .Draw();
                }
                circleBuilder.SetDefault();
            }

            // отрисовка дронов
            {
                CW_PROFILE_SCOPE("drones draw");
                for (size_t i = 0; i < m_CopyState.DronesDirections.size(); ++i)
                {
                    if (m_DebugDroneVisuals)
                    {
                        DebugDrawDirectionVisuals(m_CopyState.DronesPositions[i], m_CopyState.DronesDirections[i], m_CopyState.DronesAttractions[i]);
                        DebugDrawViewDistance(m_CopyState.DronesPositions[i], m_CopyState.DronesDirections[i], m_Settings->Drones.ViewDistance, std::acos(m_Settings->Drones.FOV));
                    }

                    m_DroneSprite->setPosition(m_CopyState.DronesPositions[i]);
                    if (abs(m_CopyState.DronesDirections[i].asRadians()) > angle::PI_2)
                    {
                        m_DroneSprite->setScale({ RESOURCE_SCALE_FACTOR, -RESOURCE_SCALE_FACTOR });
                    }
                    else
                    {
                        m_DroneSprite->setScale({ RESOURCE_SCALE_FACTOR, RESOURCE_SCALE_FACTOR });
                    }
                    m_DroneSprite->setRotation(m_CopyState.DronesDirections[i]);
                    Renderer::Get().Draw(*m_DroneSprite);
                }
            }
          
            // отрисовка ландшафта
            {
                CW_PROFILE_SCOPE("terrain draw");
                const auto& terrain = m_CopyState.Terrain;
                // генерация нового меша
                GenerateMeshes(terrain, leftGen);
                GenerateMeshes(terrain, rightGen);
                for (const auto& mesh : m_TerrainSectionMeshes)
                {
                    renderer.Draw(mesh);
                    renderer.Draw(mesh, &m_DarkeningShader);
                }
            }
        }

        // сбор состояния модели для рендера
        void CollectState(SimulationState& state)
        {
            m_Drones.CollectState(state, m_DebugDroneVisuals);
            m_Beacons.CollectState(state, m_DrawChunks);
            m_Resources.CollectState(state);
            state.ResieverPosition = m_MotherBase.GetPos();
            state.ResourceCount = m_MotherBase.GetResources();
            state.TotalResourceCount = m_MotherBase.GetTotalResources();
            state.Terrain = m_Terrain.GetTerrain();
        }

        // сбор состояния модели для сохранения
        void CollectState(FullSimulationState& state)
        {
            m_Drones.CollectState(state);
            m_Beacons.CollectState(state);
            m_Resources.CollectState(state);
            state.GeneratedRange = m_GeneratedRange;
            state.MotherBaseData = m_MotherBase.GetData();
            state.Settings = *m_Settings;
        }

    private:
        void OneIteration(float deltaTime)
        {
            GenerateChunksForDrones();

            if (m_MotherBase.Update(deltaTime))
            {
                // создание дрона
                sf::Angle randAngle = sf::degrees(lerp(0.0f, 180.0f, rand_float()));
                m_Drones.CreateDrone(m_MotherBase.GetPos(), randAngle);
                CW_TRACE("Drone spawned: count {}", m_Drones.Size());
            }

            {
                CW_PROFILE_SCOPE("beacons update");
                m_Beacons.Update(deltaTime);
            }
            {
                CW_PROFILE_SCOPE("drones update");
                std::vector<BeaconComponents> componentsVec = m_Drones.UpdateAllDrones(deltaTime, m_Resources.GetResources(),
                    m_Beacons.GetChuncks(), m_MotherBase, m_Terrain);
                for (auto [pos, type, bitDir] : componentsVec)
                {
                    // создание маяков
                    m_Beacons.CreateBeacon(pos, type, bitDir);
                }
            }
            m_ElapsedTime += deltaTime;
        }

        // функция, запускаемая в отдельном потоке
        void SimulationCycle()
        {
            while (!m_SimulationStopRequest)
            {
                OneIteration(c_FixedDeltaTime);
                m_Limiter.Wait();
            }
        }

        bool OnWindowResized(WindowResized& e)
        {
            m_WaterShader.setUniform("uResolution", static_cast<sf::Vector2f>(e.Size));
            return false;
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
        
        void SaveSimulation()
        {
            std::ofstream file(SAVES_FOLDER + m_CurrentFilePath + SAVE_EXTENSION, std::ios::binary);
            if (!file)
            {
                std::string messege = std::format("Unable to open file for saving! path: {}", m_CurrentFilePath);
                EventHandler::Get().SendEvent(MessegeToUser{ messege.c_str()});
                CW_ERROR("{}", messege);
                HandleSaveResults(true);
                return;
            }

            FullSimulationState state;
            if (!m_Paused && !m_RunSingleThread)
                SyncStopSimulationThread();
            CollectState(state);
            if (!m_Paused && !m_RunSingleThread)
                SpawnSimulationThread();

            // base
            file.write(reinterpret_cast<const char*>(&state.MotherBaseData), sizeof(state.MotherBaseData));

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

            HandleSaveResults(false);
        }

        void SwitchThread()
        {
            m_Limiter.SetUPS(m_TargetUPS);
            m_RunSingleThread = !m_RunSingleThread;
            if (m_Paused)
                return;
            if (!m_RunSingleThread)
            {
                m_Limiter.Reset();
                SpawnSimulationThread();
            }
            else
            {
                SyncStopSimulationThread();
            }
        }

        bool OnKeyPressed(KeyPressed& event)
        {
            if (event.Data.code == sf::Keyboard::Key::Space)
            {
                if (!m_RunSingleThread && !m_Paused)
                {
                    SyncStopSimulationThread();
                }
                else if (!m_RunSingleThread && m_Paused)
                {
                    m_Limiter.Reset();
                    SpawnSimulationThread();
                }
                m_Paused = !m_Paused;
            }
            return false;
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
                GenerateChunkRange(leftBorderKey - 1, m_GeneratedRange.x);
                GenerateResourcesInChunck(leftBorderKey, m_GeneratedRange.x);
                m_LeftGeneratedSections.x = leftBorderKey - 1;
                m_GeneratedRange.x = leftBorderKey - 1;
            }
            if (rightBorderKey >= m_GeneratedRange.y && cameraViewRect.contains({ rightBorderX, cameraViewRect.getCenter().y }))
            {
                rightBorderKey += 1;
                GenerateChunkRange(m_GeneratedRange.y, rightBorderKey + 1);
                GenerateResourcesInChunck(m_GeneratedRange.y, rightBorderKey);
                m_RightGeneratedSections.y = rightBorderKey + 1;
                m_GeneratedRange.y = rightBorderKey + 1;
            }
        }

        void GenerateChunkRange(int left, int right)
        {
            for (int key = left; key < right; ++key)
            {
                if (!m_Terrain.Generate(key))
                    continue;
            }
        }

        void GenerateResourcesInChunck(int left, int right)
        {
            for (int key = left; key < right; ++key)
            {
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
                GenerateChunkRange(leftBorderKey - 1, m_GeneratedRange.x);
                GenerateResourcesInChunck(leftBorderKey, m_GeneratedRange.x);
                m_LeftGeneratedSections.x = leftBorderKey - 1;
                m_GeneratedRange.x = leftBorderKey - 1;
            }
            if (rightBorderKey >= m_GeneratedRange.y)
            {
                rightBorderKey += 1;
                GenerateChunkRange(m_GeneratedRange.y, rightBorderKey + 1);
                GenerateResourcesInChunck(m_GeneratedRange.y, rightBorderKey);
                m_RightGeneratedSections.y = rightBorderKey + 1;
                m_GeneratedRange.y = rightBorderKey + 1;
            }
        }

        void DebugDrawDirectionVisuals(sf::Vector2f position, sf::Angle directionAngle, sf::Angle attractionAngle) const
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

        void DebugDrawViewDistance(sf::Vector2f position, sf::Angle directionAngle, sf::Vector2f viewDistance, float FOVRad) const
        {
            auto& FOVVisualBuilder = Renderer::Get().BeginLineShape()
                .Length(viewDistance.y)
                .Position(position);
            FOVVisualBuilder
                .SetRotationByP1(sf::radians(FOVRad))
                .RotateByP1(directionAngle)
                .Draw();
            FOVVisualBuilder.Position(position)
                .SetRotationByP1(sf::radians(-FOVRad))
                .RotateByP1(directionAngle)
                .Draw();
            FOVVisualBuilder.SetDefault();

            auto& viewDistMeshBuilder = Renderer::Get().BeginCircleShape()
                .PointCount(16)
                .Color(sf::Color::Transparent)
                .OutlineColor(sf::Color::White)
                .OutlineThickness(1.0f);
            viewDistMeshBuilder
                .Radius(viewDistance.x)
                .Position(position)
                .Draw();
            viewDistMeshBuilder
                .Radius(viewDistance.y)
                .Position(position)
                .Draw();
            viewDistMeshBuilder.SetDefault();
        }

        void UpdateDebugInterface()
        {
            ImGui::Begin("Debug");
            if (ImGui::CollapsingHeader("App-statistics"))
            {
                const auto& render = Renderer::Get().GetRenderTarget();
                sf::Vector2i mousePos = sf::Mouse::getPosition();
                ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("mouse position: (%d, %d)", mousePos.x, mousePos.y);
                ImGui::Text("world mouse position: (%.2f, %.2f)",
                    render.mapPixelToCoords(mousePos).x,
                    render.mapPixelToCoords(mousePos).y);
                ImGui::Text("render target view size: (%.2f, %.2f)", render.getView().getSize().x, render.getView().getSize().y);
                ImGui::Text("render target view center: (%.2f, %.2f)", render.getView().getCenter().x, render.getView().getCenter().y);
                ImGui::Spacing();
            }
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Virtual-time"))
            {
                if (m_RunSingleThread)
                {
                    ImGui::SliderInt("Traget UPS", &m_TargetUPS, 60, 1024);
                }
                if (ImGui::Button("Switch virtual time"))
                {
                    SwitchThread();
                }
            }
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Simulation-statistics"))
            {
                ImGui::Text("Paused: %d", m_Paused);
                ImGui::Text("Simulation running: %d", m_SimulationRunning);
                ImGui::Separator();
                ImGui::Text("Alive drones:              %d", m_CopyState.DronesPositions.size());
                ImGui::Text("Alive beacons:             %d", m_CopyState.BeaconsPositions.size());
                ImGui::Text("Resources on base:         %d", m_CopyState.ResourceCount);
                ImGui::Text("Total resources collected: %d", m_CopyState.TotalResourceCount);
            }
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Debug"))
            {
                ImGui::Checkbox("Drone debug visuals", &m_DebugDroneVisuals);
                ImGui::Checkbox("Chuck debug visuals", &m_DrawChunks);
            }
            
            ImGui::End();
        }

        void UpdateMenuInterface()
        {
            ImGui::Begin("Menu", nullptr, WINDOW_FLAGS);
            ImGui::InputText("Path", g_FilePathBuff, sizeof(g_FilePathBuff) / sizeof(*g_FilePathBuff));

            ImGui::SameLine();
            if (ImGui::SmallButton("clear"))
            {
                g_FilePathBuff[0] = '\0';
            }

            std::string savePath = (g_FilePathBuff[0]) ? g_FilePathBuff : m_CurrentFilePath.c_str();
            ImGui::Text("Current path: %s", m_CurrentFilePath.c_str());
            if (savePath[0])
            {
                ImGui::Text("Will be saved in: %s", SAVES_FOLDER + savePath + SAVE_EXTENSION);
                if (ImGui::Button("Save"))
                {
                    m_CurrentFilePath = savePath;
                    SaveSimulation();
                }
                ImGui::SameLine();
                if (ImGui::Button("Save and Exit"))
                {
                    m_CurrentFilePath = savePath;
                    SaveSimulation();
                    g_FilePathBuff[0] = '\0';
                    m_ExitFlag = ExitStatus::SaveAndExit;
                }
            }
            else
            {
                ImGui::Text("Please enter save path");
            }
            if (ImGui::Button("Exit"))
            {
                m_ExitFlag = ExitStatus::Exit;
                m_MenuIsOpen = false;
                m_DebugIsOpen = false;
                g_FilePathBuff[0] = '\0';
            }
            ImGui::End();
        }

        void HandleSaveResults(bool error)
        {
            if (error)
            {
                m_ExitFlag = ExitStatus::Error;
            }
            else
            {
                switch (m_ExitFlag)
                {
                case ExitStatus::SaveAndExit:
                    m_ExitFlag = ExitStatus::Exit;
                    m_MenuIsOpen = false;
                    m_DebugIsOpen = false;
                    m_Paused = false;
                    break;
                case ExitStatus::JustSaving:  m_ExitFlag = ExitStatus::None; break;
                default: break;
                }
            }
        }

    private:
        std::thread m_SimulationThread;
        const float c_FixedDeltaTime = 1.0f / 60.0f;
        std::atomic<bool> m_SimulationStopRequest = false;

        UPSLimiter m_Limiter{ 60 };
        std::string m_CurrentFilePath = "";
        int m_TargetUPS = 60;
        bool m_RunSingleThread = true;
        bool m_SimulationRunning = false;
        bool m_Paused = false;

        SimulationState m_CopyState;

        Camera2D m_Camera;

        Shared<SimulationSettings> m_Settings;

        BeaconManager m_Beacons;
        DroneManager m_Drones;
        ResourceManager m_Resources;
        MotherBase m_MotherBase;

        TerrainGenerator m_Terrain;
        sf::Vector2i m_GeneratedRange{-5, 5};
        sf::Vector2i m_LeftGeneratedSections{ 0, 0 };
        sf::Vector2i m_RightGeneratedSections{ 0, 0 };

        std::vector<sf::ConvexShape> m_TerrainSectionMeshes;
        sf::Texture m_TerrainTexture;

        float m_ElapsedTime = 0.0f;

        // Menu
        bool m_MenuIsOpen = false;
        bool m_DebugIsOpen = false;

        ExitStatus m_ExitFlag = ExitStatus::None;

        // Visuals
        sf::Shader m_WaterShader;
        sf::Shader m_DarkeningShader;

        Unique<sf::Sprite> m_DroneSprite;
        Unique<sf::Texture> m_DroneTexture;

        std::array<Unique<sf::Sprite>, 3> m_ResourceSprites;
        std::array<Unique<sf::Texture>, 3> m_ResourceTextures;

        Unique<sf::Sprite> m_MotherBaseSprite;
        Unique<sf::Texture> m_MotherBaseTexture;

        // Debug
        bool m_DebugDroneVisuals = false;
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
        enum class HoverStatus
        {
            None = 0,
            Start,
            Load,
            Exit
        };

    public:
        MainMenuLayer(sf::Vector2f windowSize, Shared<SimulationSettings> simSettings)
            : m_StartButtonCollision(CreateShared<RectCollision>(sf::Vector2f{ windowSize.x / 2.0f - 50.0f, windowSize.y / 2.0f - 100.0f }, sf::Vector2f{ 100.0f, 50.0f })),
              m_LoadButtonCollision(CreateShared<RectCollision>(sf::Vector2f{ windowSize.x / 2.0f - 50.0f, windowSize.y / 2.0f }, sf::Vector2f{ 100.0f, 50.0f })),
              m_ExitButtonCollision(CreateShared<RectCollision>(sf::Vector2f{ windowSize.x / 2.0f - 50.0f, windowSize.y / 2.0f + 100.0f }, sf::Vector2f{ 100.0f, 50.0f })),
              m_SimSettings(simSettings)
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
                        EventHandler::Get().SendEvent(CloseApp{});
                    return true;
                });

            m_StartSimButton.SetOnEnterCallback(
                [this](MouseMoved&) {
                    m_HoverStatus = HoverStatus::Start;
                    return true;
                });
            m_LoadButton.SetOnEnterCallback(
                [this](MouseMoved&) {
                    m_HoverStatus = HoverStatus::Load;
                    return true;
                });
            m_ExitButton.SetOnEnterCallback(
                [this](MouseMoved&) {
                    m_HoverStatus = HoverStatus::Exit;
                    return true;
                });

            m_StartSimButton.SetOnExitCallback(
                [this](MouseMoved&) {
                    m_HoverStatus = HoverStatus::None;
                    return true;
                });
            m_LoadButton.SetOnExitCallback(
                [this](MouseMoved&) {
                    m_HoverStatus = HoverStatus::None;
                    return true;
                });
            m_ExitButton.SetOnExitCallback(
                [this](MouseMoved&) {
                    m_HoverStatus = HoverStatus::None;
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
            sf::Color startHighlight = sf::Color::White;
            sf::Color loadHighlight  = sf::Color::White;
            sf::Color exitHighlight  = sf::Color::White;
            switch (m_HoverStatus)
            {
            case HoverStatus::Start: startHighlight = sf::Color{ 230, 230, 210, 255 }; break;
            case HoverStatus::Load:  loadHighlight  = sf::Color{ 230, 230, 210, 255 }; break;
            case HoverStatus::Exit:  exitHighlight  = sf::Color{ 230, 230, 210, 255 }; break;
            case HoverStatus::None:  startHighlight = loadHighlight = exitHighlight = sf::Color::White; break;
            }
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
                .Color(startHighlight)
                .Draw();
            // Load
            renderer.BeginRectangleShape()
            .Size(m_LoadButtonCollision->GetSize())
                .Position(m_LoadButtonCollision->GetPos())
                .Texture(&m_LoadTexture)
                .OutlineThickness(2.0f)
                .OutlineColor({ 14, 77, 148 })
                .Color(loadHighlight)
                .Draw();
            // Exit
            renderer.BeginRectangleShape()
                .Size(m_ExitButtonCollision->GetSize())
                .Position(m_ExitButtonCollision->GetPos())
                .Texture(&m_ExitTexture)
                .OutlineThickness(2.0f)
                .OutlineColor({ 240, 25, 25 })
                .Color(exitHighlight)
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

        // начальные настройки симуляции
        void SimulationSettingsMenu()
        {
            sf::Vector2f windowSize = Renderer::Get().GetWindowSize();
            ImGui::SetNextWindowSize({ 550.0f, 500.0f }, ImGuiCond_Appearing);
            ImGui::SetNextWindowPos({ windowSize.x / 2.0f - 275.0f, windowSize.y / 2.0f - 250.0f}, ImGuiCond_Appearing);
            bool simSetWind = (m_MenuStatus == MenuStatus::SimSetting) ? true : false;
            if (ImGui::Begin("Simulation settings", &simSetWind, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
            {
                float height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
                ImGui::BeginChild("SturtupSettings", ImVec2(ImGui::GetContentRegionAvail().x, -height));
                
                DroneSetupInterface();
                BeaconSetupInterface();
                TerrainSetupInterface();

                ImGui::EndChild();

                ImGui::Separator();
                if (ImGui::Button("Start simulation"))
                {
                    EventHandler::Get().SendEvent(StartSimulation{});
                }
                ImGui::End();
            }
            if (!simSetWind)
                m_MenuStatus = MenuStatus::None;
        }

        void TerrainSetupInterface() const
        {
            if (ImGui::CollapsingHeader("Terrain"))
            {
                ImGui::Separator();
                auto& terrain = m_SimSettings->TerrainGenerator;
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

        void BeaconSetupInterface() const
        {
            if (ImGui::CollapsingHeader("Beacons"))
            {
                ImGui::Separator();
                auto& beacons = m_SimSettings->Beacons;
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
                auto& drones = m_SimSettings->Drones;
                ImGui::Separator();
                ImGui::InputInt("Drones count", &m_SimSettings->DronesCount);
                ImGui::InputFloat("Starting position", &m_SimSettings->StartingHorizontalPosition);
                ImGui::Spacing();

                ImGui::InputFloat("Drone discharge rate (% / sec)", &drones.DischargeRate, 1.0f, 10.0f);
                ImGui::InputFloat("Drone spawn cooldown", &drones.BeaconCooldownSec, 1.0f, 10.0f);
                ImGui::Spacing();
                ImGui::InputFloat("Speed", &drones.Speed, 10.0f, 100.0f);
                ImGui::SliderAngle("Turning speed", &drones.TurningSpeed, 1.0f, 180.0f);
                ImGui::InputFloat("FOV", &drones.FOV, 0.01f, 0.1f);
                ImGui::InputFloat2("View distance", &drones.ViewDistance.x);
                ImGui::Spacing();
                ImGui::InputFloat("Beacon spawn cooldown", &drones.BeaconCooldownSec, 0.5f, 1.0f);
                ImGui::InputFloat("Wander cooldown", &drones.BeaconCooldownSec, 0.5f, 1.0f);
                ImGui::Spacing();
                ImGui::SliderAngle("Random wander angle", &drones.RandomWanderAngleRad, 1.0f, 180.0f);
                ImGui::SliderFloat("Wander angle threashold", &drones.WanderAngleThresholdDeg, 0.0f, 15.0f);
                ImGui::SliderAngle("Max turning delta", &drones.MaxTurningDeltaRad, 1.0f, 180.0f);
                ImGui::Spacing();
                ImGui::Checkbox("Bit directions", &drones.BitDirections);

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
            ImGui::SetNextWindowPos({ 250.0f, 150.0f }, ImGuiCond_Appearing);
            if (ImGui::Begin("Load save", &simLoadWind, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
            {
                ImGui::InputText("Path", g_FilePathBuff, sizeof(g_FilePathBuff) / sizeof(*g_FilePathBuff));
                if (g_FilePathBuff[0])
                {
                    if (ImGui::Button("Load"))
                    {
                        EventHandler::Get().SendEvent(LoadSimulation{ g_FilePathBuff });
                        g_FilePathBuff[0] = '\0';
                    }
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
        HoverStatus m_HoverStatus = HoverStatus::None;

        Shared<SimulationSettings> m_SimSettings;
    };


    class InterfaceLayer
        : public Layer
    {
    public:
        enum class HoverStatus
        {
            None = 0,
            Menu,
            Debug,
        };
    public:
        InterfaceLayer(sf::Vector2f windowSize)
            : m_MenuButtonCollision(CreateShared<CircleCollision>(sf::Vector2f{windowSize.x - 35.0f, 35.0f}, 25.0f)),
              m_DebugMenuButtonCollision(CreateShared<CircleCollision>(sf::Vector2f{ windowSize.x - 35.0f, 95.0f }, 25.0f))
        {
            m_MenuButton.SetCollisionChecker(m_MenuButtonCollision);
            m_DebugMenuButton.SetCollisionChecker(m_DebugMenuButtonCollision);

            m_MenuButton.SetOnClickCallback(
                [](MouseButtonPressed&) {
                    EventHandler::Get().SendEvent(SwitchMenu{});
                    return true;
                });
            m_DebugMenuButton.SetOnClickCallback(
                [](MouseButtonPressed&) {
                    EventHandler::Get().SendEvent(SwitchDebugMenu{});
                    return true;
                });

            m_MenuButton.SetOnEnterCallback(
                [this](MouseMoved&) {
                    m_HoverStatus = HoverStatus::Menu;
                    return true;
                });
            m_DebugMenuButton.SetOnEnterCallback(
                [this](MouseMoved&) {
                    m_HoverStatus = HoverStatus::Debug;
                    return true;
                });

            m_MenuButton.SetOnExitCallback(
                [this](MouseMoved&) {
                    m_HoverStatus = HoverStatus::None;
                    return true;
                });
            m_DebugMenuButton.SetOnExitCallback(
                [this](MouseMoved&) {
                    m_HoverStatus = HoverStatus::None;
                    return true;
                });

            m_UpdateActive = false;

            if (!m_MenuTexture.loadFromFile(SPRITES_FOLDER "menu_button.png"))
                CW_ERROR("Unable to load menu button texture!");
            if (!m_DebugTexture.loadFromFile(SPRITES_FOLDER "debug_button.png"))
                CW_ERROR("Unable to load debug button texture!");
        }

        void Draw() override
        {
            sf::Color menuHighlight = sf::Color::White;
            sf::Color debugHighlight = sf::Color::White;
            switch (m_HoverStatus)
            {
            case HoverStatus::Menu:  menuHighlight  = sf::Color{ 230, 230, 210, 255 }; break;
            case HoverStatus::Debug: debugHighlight = sf::Color{ 230, 230, 210, 255 }; break;
            case HoverStatus::None:  menuHighlight = debugHighlight = sf::Color::White; break;
            }

            auto& renderer = Renderer::Get();
            renderer.ApplyDefaultView();
            // menu button
            renderer.BeginCircleShape()
                .Radius(m_MenuButtonCollision->GetRadius())
                .Position(m_MenuButtonCollision->GetPos())
                .Texture(&m_MenuTexture)
                .Color(menuHighlight)
                .Draw();
            // debug menu button
            renderer.BeginCircleShape()
                .Radius(m_DebugMenuButtonCollision->GetRadius())
                .Position(m_DebugMenuButtonCollision->GetPos())
                .Texture(&m_DebugTexture)
                .Color(debugHighlight)
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
        HoverStatus m_HoverStatus = HoverStatus::None;

        sf::Texture m_MenuTexture;
        sf::Texture m_DebugTexture;
    };


    class MyApp
        : public Application
    {
    public:
        MyApp()
            : Application(800, 600, "test"),
              m_SimSettings(CreateShared<SimulationSettings>())
        {
            m_ClearColor = sf::Color(0, 0, 0, 255);
        }

        void Init() override
        {
            PushLayer<MainMenuLayer>((sf::Vector2f) GetWindowSize(), m_SimSettings);
        }

        void Update(float deltaTime) override
        {
            AppIteration();
        }

        void PausedUpdate(float) override
        {
            AppIteration();
        }

        void Draw() override
        {
            CW_PROFILE_FUNCTION();
        }

        void OnEvent(Event& event) override
        {
            EventDispatcher dispatcher(event);
            if (dispatcher.Dispach<CloseApp>(CW_BUILD_EVENT_FUNC(OnCloseApp)))
                return;
            if (dispatcher.Dispach<StartSimulation>(CW_BUILD_EVENT_FUNC(OnStartSimulation)))
                return;
            if (dispatcher.Dispach<SimulationOver>(CW_BUILD_EVENT_FUNC(OnSimulationOver)))
                return;
            if (dispatcher.Dispach<LoadSimulation>(CW_BUILD_EVENT_FUNC(OnLoadSimulation)))
                return;
            if (dispatcher.Dispach<MessegeToUser>(CW_BUILD_EVENT_FUNC(OnMessegeToUser)))
                return;
            
            dispatcher.Dispach<WindowResized>(CW_BUILD_EVENT_FUNC(OnWindowResized));
            dispatcher.Dispach<KeyPressed>(CW_BUILD_EVENT_FUNC(OnKeyPressed));
            OnEventLayers(event);
        }

    private:
        void AppIteration()
        {
            CW_PROFILE_FUNCTION();
            
            if (m_PopupIsOpen)
                UpdatePopup();
        }

        bool OnKeyPressed(KeyPressed& event)
        {
            if (m_SimIsRunning && event.Data.code == sf::Keyboard::Key::Space)
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
            PopLayer(); // удаление MainMenuLayer
            PushLayer<SimulationLayer>((sf::Vector2f) GetWindowSize(), m_SimSettings);
            PushLayer<InterfaceLayer>((sf::Vector2f) GetWindowSize());
            m_ClearColor = sf::Color(135, 206, 235, 255);
            m_SimIsRunning = true;
            return true;
        }

        bool OnSimulationOver(SimulationOver&)
        {
            PopLayer(); // удаление InterfceLayer
            PopLayer(); // удаление SimulationLayer
            PushLayer<MainMenuLayer>((sf::Vector2f)GetWindowSize(), m_SimSettings);
            m_ClearColor = sf::Color(0, 0, 0, 255);
            m_SimIsRunning = false;
            m_Pause = false;
            return true;
        }

        bool OnLoadSimulation(LoadSimulation& e)
        {
            std::ifstream file(SAVES_FOLDER + e.FilePath + SAVE_EXTENSION, std::ios::binary);
            if (!file)
            {
                m_PopupMessege = std::format("Unable to open file for loading: {}", e.FilePath);
                m_PopupIsOpen = true;
                CW_ERROR("{}", m_PopupMessege);
                return true;
            }

            FullSimulationState state;

            // base
            file.read(reinterpret_cast<char*>(&state.MotherBaseData), sizeof(state.MotherBaseData));

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
            *m_SimSettings = state.Settings;
            PushLayer<SimulationLayer>((sf::Vector2f) GetWindowSize(), m_SimSettings, &state, e.FilePath.c_str());
            PushLayer<InterfaceLayer>((sf::Vector2f) GetWindowSize());
            m_ClearColor = sf::Color(135, 206, 235, 255);
            m_SimIsRunning = true;

            CW_TRACE("Simulation loaded successfully: {}", e.FilePath);

            return true;
        }

        bool OnMessegeToUser(MessegeToUser& e)
        {
            m_PopupIsOpen = true;
            m_PopupMessege = std::move(e.Messege);
            return true;
        }

        void UpdatePopup()
        {
            ImGui::Begin("Messege", &m_PopupIsOpen, WINDOW_FLAGS);
            ImGui::Text(m_PopupMessege.c_str());
            if (ImGui::Button("Ok"))
                m_PopupIsOpen = false;
            ImGui::End();
        }

    private:
        bool m_SimIsRunning = false;

        bool m_PopupIsOpen = false;
        std::string m_PopupMessege = "";

        Shared<SimulationSettings> m_SimSettings;
    };

} // CW

std::unique_ptr<CW::Application> create_program(int argc, const char** argv)
{
    return std::make_unique<CW::MyApp>();
}
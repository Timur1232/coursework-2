//#ifdef CW_TEST
#include "pch.h"

#if 1
#include <engine/EntryPoint.h>

#include <engine/ProgramCore.h>

#include "debug_utils/Log.h"
#include "debug_utils/Profiler.h"
#include "utils/ArenaAllocator.h"

#include "Camera2D.h"
#include "Beacon.h"
#include "Drone.h"

namespace CW {

    constexpr size_t BEACONS_RESERVE = 1024 * 1024;

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
            m_Resources.emplace_back(sf::Vector2f{ 0.0f, 0.0f });

            Drone::StaticInit();
            Beacon::StaticInit();
        }

        void RestartSim(size_t droneCount, sf::Vector2f startPosition = { 0.0f, 0.0f }, TargetType target = TargetType::Recource)
        {
            m_Beacons.clear();
            m_AllocatorBeacon.Deallocate();
            m_Beacons.reserve(BEACONS_RESERVE);
            m_DeadBeacons = 0;

            m_Drones.clear();
            m_Drones.reserve(droneCount);
            float angleStep = 2.0f * angle::PI / (float)droneCount;
            float angle = 0.0f;
            for (size_t i = 0; i < droneCount; ++i, angle += angleStep)
            {
                m_Drones.emplace_back(startPosition, sf::radians(angle), target);
            }
            CW_TRACE("Restarting simulation with {} drones", droneCount);
        }

        void Update(sf::Time deltaTime) override
        {
            CW_PROFILE_FUNCTION();
            ImGui::Begin("Debug");
                if (ImGui::CollapsingHeader("App statistics"))
                {
                    ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
                    ImGui::Spacing();
                    ImGui::Text("m_Beacons size: %d", m_Beacons.size());
                    ImGui::Text("m_Beacons capasity: %d", m_Beacons.capacity());
                    ImGui::Text("allocator current allocated: %d", m_AllocatorBeacon.CurrentAllocated());
                    ImGui::Text("allocator current capasity: %d", m_AllocatorBeacon.CurrentCapasity());
                    ImGui::Text("allocator total allocated: %d", m_AllocatorBeacon.TotalAllocated());
                    ImGui::Text("allocator total capasity: %d", m_AllocatorBeacon.TotalCapasity());
                    ImGui::Text("allocator blocks count: %d", m_AllocatorBeacon.BlockCount());
                    ImGui::Spacing();
                    ImGui::Text("mouse hovering on any window: %d", ImGui::GetIO().WantCaptureMouse);
                    m_Camera.DebugInterface();
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
                    if (ImGui::RadioButton("Recource", target == TargetType::Recource))
                        target = TargetType::Recource;
                    ImGui::SameLine();
                    if (ImGui::RadioButton("Navigation", target == TargetType::Navigation))
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
                        Beacon::StaticInit();

                    ImGui::Checkbox("show beacons info", &m_BeaconsInfo);
                    Beacon::DebugInterface();
                }
            ImGui::End();

            m_Camera.Update(deltaTime);

            {
                CW_PROFILE_SCOPE("beacons update");
                for (size_t i = 0; i < m_Beacons.size() - m_DeadBeacons; ++i)
                {
                    m_Beacons[i]->Update(deltaTime);
                    if (m_BeaconsInfo)
                        m_Beacons[i]->InfoInterface(i, &m_BeaconsInfo);

                    if (!m_Beacons[i]->IsAlive())
                    {
                        ++m_DeadBeacons;
                        std::swap(m_Beacons[i], m_Beacons[m_Beacons.size() - m_DeadBeacons]);
                    }
                }
            }

            {
                CW_PROFILE_SCOPE("drones update");
                size_t index = 0;
                for (auto& drone : m_Drones)
                {
                    drone.Update(deltaTime);
                    if (!drone.ReactToResourceReciver(m_ResourceReciever))
                        drone.ReactToBeacons(m_Beacons);
                    drone.ReactToResources(m_Resources);
                    if (m_DronesInfo)
                        drone.InfoInterface(index, &m_DronesInfo);
                    ++index;
                }
            }
        }

        void Draw(sf::RenderWindow& render) const override
        {
            CW_PROFILE_FUNCTION();
            render.setView(m_Camera.GetView());

            m_ResourceReciever.Draw(render);
            
            for (const auto& resource : m_Resources)
            {
                resource.Draw(render);
            }

            {
                CW_PROFILE_SCOPE("beacons draw");
                for (const auto& b : m_Beacons)
                {
                    b->Draw(render);
                }
            }

            {
                CW_PROFILE_SCOPE("drones draw");
                for (const auto& drone : m_Drones)
                {
                    drone.Draw(render);
                }
            }
        }

        void OnClosed() override
        {
            Close();
        }

        void OnKeyPressed(const sf::Event::KeyPressed* event) override
        {
        }

        void OnMouseButtonPressed(const sf::Event::MouseButtonPressed* e) override
        {
            if (!m_Hold && !ImGui::GetIO().WantCaptureMouse && e->button == sf::Mouse::Button::Left)
            {
                createBeacon(m_Camera.WorldPosition(e->position), TargetType::Recource);
                m_Hold = true;
            }
        }

        void OnMouseButtonReleased(const sf::Event::MouseButtonReleased* e) override
        {
            m_Hold = m_Hold && e->button != sf::Mouse::Button::Left;
        }

        void OnCreateBeacon(const CreateBeacon* e) override
        {
            createBeacon(e->Position, e->Type);
        }

    private:
        void createBeacon(sf::Vector2f position, TargetType type)
        {
            CW_PROFILE_FUNCTION();
            if (m_DeadBeacons)
            {
                m_Beacons[m_Beacons.size() - m_DeadBeacons]->Revive(position, type);
                --m_DeadBeacons;
            }
            else
            {
                Beacon* newBeacon = m_AllocatorBeacon.Allocate();
                newBeacon->Revive(position, type);
                m_Beacons.push_back(newBeacon);
            }
        }

    private:
        Camera2D m_Camera;
        bool m_Hold = false;

        std::vector<Beacon*> m_Beacons;
        size_t m_DeadBeacons = 0;
        ArenaAllocator<Beacon> m_AllocatorBeacon{1024};

        std::vector<Drone> m_Drones;

        ResourceReciever m_ResourceReciever{ {1000.0f, 1000.0f} };
        std::vector<Resource> m_Resources;

        // Debug
        bool m_BeaconsInfo = false;
        bool m_DronesInfo = false;
    };

} // CW

std::unique_ptr<CW::Application> create_program(int argc, const char** argv)
{
    return std::make_unique<CW::MyApp>();
}

#else

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "debug_utils/Log.h"

#include <ranges>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "hui");



    window.setFramerateLimit(60);

    float radius = 10.0f;
    sf::CircleShape shape(radius);
    shape.setFillColor(sf::Color::Green);

    if (!ImGui::SFML::Init(window))
    {
        return -1;
    }

    bool showShape = true;
    int counter = 0;
    sf::Clock deltaClock;
    auto& io = ImGui::GetIO();

    sf::View camera{ {400, 300}, {800, 600} };
    bool cameraMoving = false;
    sf::Vector2i prevMousePos{ 0, 0 };
    float zoomFactor = 1.0f;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
                break;
            }

            else if (auto e = event->getIf<sf::Event::MouseButtonPressed>())
            {
                cameraMoving = e->button == sf::Mouse::Button::Right;
            }

            else if (auto e = event->getIf<sf::Event::MouseButtonReleased>())
            {
                cameraMoving = e->button != sf::Mouse::Button::Right && cameraMoving;
            }

            else if (auto e = event->getIf<sf::Event::MouseMoved>())
            {
                if (cameraMoving)
                    camera.move((sf::Vector2f)(prevMousePos - e->Position) * zoomFactor);

                prevMousePos = e->Position;
            }

            else if (auto e = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (e->delta < 0)
                {
                    camera.zoom(1.1f);
                    zoomFactor *= 1.1f;
                }

                else if (e->delta > 0)
                {
                    camera.zoom(0.9f);
                    zoomFactor *= 0.9f;
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Debug");
        ImGui::Checkbox("Show shape", &showShape);
        if (showShape)
        {
            ImGui::SliderFloat("radius", &radius, 10.0f, 200.0f);
            shape.setRadius(radius);
        }

        ImGui::Text("camera position: (%.2f, %.2f)", camera.getCenter().x, camera.getCenter().y);
        ImGui::Text("camera size: (%.2f, %.2f)", camera.getSize().x, camera.getSize().y);
        ImGui::Text("zoom factor: %.2f", zoomFactor);

        ImGui::End();

        window.clear();

        window.setView(camera);

        shape.setPosition({ 0.0f, 0.0f });
        if (showShape)
            window.draw(shape);

        shape.setPosition({10.0f, 100.0f});
        if (showShape)
            window.draw(shape);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}

#endif

//#endif
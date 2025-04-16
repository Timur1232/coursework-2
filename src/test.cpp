#ifdef CW_TEST

#if 1
#include <engine/EntryPoint.h>

#include <engine/ProgramCore.h>
#include <imgui.h>
#include <imgui-SFML.h>

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
          //public OnKeyPressed,
          public OnClosed,
          public OnMouseButtonPressed,
          public OnMouseButtonReleased,
          public OnCreateBeacon
    {
    public:
        MyApp()
            : Application(800, 600, "test"),
              m_Camera(0, 0, 800, 600)
        {
            m_Camera.subscribeOnEvents();

            Drone::staticInit();
            Beacon::staticInit();
        }

        void restartSim(size_t droneCount, sf::Vector2f startPosition = { 0.0f, 0.0f }, TargetType target = TargetType::Recource)
        {
            m_Beacons.clear();
            m_AllocatorBeacon.deallocate();
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

        void update(sf::Time deltaTime) override
        {
            CW_PROFILE_FUNCTION();
            ImGui::Begin("Debug");
                if (ImGui::CollapsingHeader("App statistics"))
                {
                    ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
                    ImGui::Spacing();
                    ImGui::Text("m_Beacons size: %d", m_Beacons.size());
                    ImGui::Text("m_Beacons capasity: %d", m_Beacons.capacity());
                    ImGui::Text("allocator current allocated: %d", m_AllocatorBeacon.currentAllocated());
                    ImGui::Text("allocator current capasity: %d", m_AllocatorBeacon.currentCapasity());
                    ImGui::Text("allocator total allocated: %d", m_AllocatorBeacon.totalAllocated());
                    ImGui::Text("allocator total capasity: %d", m_AllocatorBeacon.totalCapasity());
                    ImGui::Text("allocator blocks count: %d", m_AllocatorBeacon.blockCount());
                    ImGui::Spacing();
                    ImGui::Text("mouse hovering on any window: %d", ImGui::GetIO().WantCaptureMouse);
                    m_Camera.debugInterface();
                }

                if (ImGui::CollapsingHeader("Resources"))
                {
                    m_ResourceReciever.debugInterface();
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
                        restartSim(droneCount, startPos, target);
                    }

                    ImGui::Checkbox("show drones info", &m_DronesInfo);
                    Drone::debugInterface();

                    ImGui::Text("default drones settings");
                    if (ImGui::Button("default drone"))
                        Drone::staticInit();
                }

                if (ImGui::CollapsingHeader("Beacons"))
                {
                    ImGui::Text("default beacons settings");
                    if (ImGui::Button("default beacon"))
                        Beacon::staticInit();

                    ImGui::Checkbox("show beacons info", &m_BeaconsInfo);
                    Beacon::debugInterface();
                }
            ImGui::End();

            m_Camera.update(deltaTime);

            {
                CW_PROFILE_SCOPE("beacons update");
                for (size_t i = 0; i < m_Beacons.size() - m_DeadBeacons; ++i)
                {
                    m_Beacons[i]->update(deltaTime);
                    if (m_BeaconsInfo)
                        m_Beacons[i]->infoInterface(i, &m_BeaconsInfo);

                    if (!m_Beacons[i]->isAlive())
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
                    drone.update(deltaTime);
                    if (!drone.reactToResourceReciver(m_ResourceReciever))
                        drone.reactToBeacons(m_Beacons);
                    if (m_DronesInfo)
                        drone.infoInterface(index, &m_DronesInfo);
                    ++index;
                }
            }
        }

        void draw(sf::RenderWindow& render) const override
        {
            CW_PROFILE_FUNCTION();
            render.setView(m_Camera.getView());

            m_ResourceReciever.draw(render);

            {
                CW_PROFILE_SCOPE("beacons draw");
                for (const auto& b : m_Beacons)
                {
                    b->draw(render);
                }
            }

            {
                CW_PROFILE_SCOPE("drones draw");
                for (const auto& drone : m_Drones)
                {
                    drone.draw(render);
                }
            }
        }

        void onClosed() override
        {
            close();
        }

        /*void onKeyPressed(const sf::Event::KeyPressed* event) override
        {
            
        }*/

        void onMouseButtonPressed(const sf::Event::MouseButtonPressed* e) override
        {
            if (!m_Hold && !ImGui::GetIO().WantCaptureMouse && e->button == sf::Mouse::Button::Left)
            {
                createBeacon(m_Camera.worldPosition(e->position), TargetType::Recource);
                m_Hold = true;
            }
        }

        void onMouseButtonReleased(const sf::Event::MouseButtonReleased* e) override
        {
            m_Hold = m_Hold && e->button != sf::Mouse::Button::Left;
        }

        void onCreateBeacon(const CreateBeacon* e) override
        {
            createBeacon(e->position, e->type);
        }

    private:
        void createBeacon(sf::Vector2f position, TargetType type)
        {
            CW_PROFILE_FUNCTION();
            if (m_DeadBeacons)
            {
                m_Beacons[m_Beacons.size() - m_DeadBeacons]->revive(position, type);
                --m_DeadBeacons;
            }
            else
            {
                Beacon* newBeacon = m_AllocatorBeacon.allocate();
                newBeacon->revive(position, type);
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

        ResourceReciever m_ResourceReciever{ {0.0f, 0.0f} };

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
                    camera.move((sf::Vector2f)(prevMousePos - e->position) * zoomFactor);

                prevMousePos = e->position;
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

#endif
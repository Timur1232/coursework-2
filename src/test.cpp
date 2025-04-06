#ifdef CW_TEST

#if 1
#include <engine/EntryPoint.h>

#include <engine/ProgramCore.h>
#include <imgui.h>
#include <imgui-SFML.h>

#include "debug_utils/Log.h"

#include "Camera2D.h"
#include "Beacon.h"
#include "Drone.h"

namespace CW {

    class MyApp
        : public CW_E::Application,
          public CW_E::OnKeyPressed,
          public CW_E::OnClosed,
          public CW_E::OnMouseButtonPressed,
          public CW_E::OnMouseButtonReleased,
          public OnCreateBeacon
    {
    public:
        MyApp()
            : Application(1920, 1080, "test"),
              m_Camera(0, 0, 1920, 1080)
        {
            m_Camera.subscribeOnEvents();

            m_Drones.emplace_back(sf::Vector2f{ 0.0f, 0.0f }, sf::Vector2f{ 0.0f, 1.0f });
            m_Drones.emplace_back(sf::Vector2f{ 0.0f, 200.0f }, sf::Vector2f{ -1.0f, 1.0f }.normalized());
        }

        void update(sf::Time deltaTime) override
        {
            ImGui::Begin("Debug");
            ImGui::Text("m_Beacons size: %d", m_Beacons.size());
            ImGui::Text("mouse hovering on any window: %d", ImGui::GetIO().WantCaptureMouse);
            ImGui::End();

            m_Camera.update(deltaTime);

            for (auto& b : m_Beacons)
            {
                b.update(deltaTime);
            }

            for (auto& drone : m_Drones)
            {
                drone.update(deltaTime);
                drone.reactToBeacons(m_Beacons);
            }
        }

        void draw(sf::RenderWindow& render) const override
        {
            render.setView(m_Camera.getView());

            for (const auto& b : m_Beacons)
            {
                b.draw(render);
            }

            for (auto& drone : m_Drones)
            {
                drone.draw(render);
            }
        }

        void onClosed() override
        {
            close();
        }

        void onKeyPressed(const sf::Event::KeyPressed* event) override
        {
            if (event->code == sf::Keyboard::Key::F11)
            {
                
            }
        }

        void onMouseButtonPressed(const sf::Event::MouseButtonPressed* e) override
        {
            if (!m_Hold && !ImGui::GetIO().WantCaptureMouse && e->button == sf::Mouse::Button::Left)
            {
                createBeacon(m_Camera.worldPosition(e->position), BeaconType::Recource);
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
        void createBeacon(sf::Vector2f position, BeaconType type)
        {
            for (auto& beacon : m_Beacons)
            {
                if (!beacon.isAlive())
                {
                    beacon.revive(position, type);
                    return;
                }
            }
            m_Beacons.emplace_back(position, type);
        }

    private:
        Camera2D m_Camera;
        bool m_Hold = false;
        std::vector<Beacon> m_Beacons;
        std::vector<Drone> m_Drones;
    };

} // CW

std::unique_ptr<CW_E::Application> create_program(int argc, const char** argv)
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

    float radius = 100.0f;
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
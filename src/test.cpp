#ifdef CW_TEST

#if 1
#include <engine/EntryPoint.h>

#include <engine/ProgramCore.h>
#include <imgui.h>
#include <imgui-SFML.h>

#include "debug_utils/Log.h"

#include "Camera2D.h"

#include "Beacon.h"

namespace CW {

    class MyApp
        : public CW_E::Application,
          public CW_E::OnKeyPressed,
          public CW_E::OnClosed,
          public CW_E::OnMouseButtonPressed,
          public CW_E::OnMouseButtonReleased,
          public OnBeaconDischarge
    {
    public:
        MyApp(CW_E::EventHandlerWrapper eventHandler, CW_E::UpdateHandlerWrapper updateHandler)
            : Application(800, 600, "test", eventHandler, updateHandler),
              radius(100.0f), shape(radius),
              m_Camera(0, 0, 800, 600, eventHandler, updateHandler)
        {
            shape.setFillColor(sf::Color::Green);
            eventHandler.subscribe(this);
        }

        void update(sf::Time deltaTime) override
        {
            ImGui::Begin("Debug");
            ImGui::Checkbox("Show shape", &showShape);
            if (showShape)
            {
                ImGui::SliderFloat("radius", &radius, 10.0f, 200.0f);
                shape.setRadius(radius);
            }
            ImGui::Text("m_Beacons size: %d", m_Beacons.size());
            ImGui::End();
        }

        void draw(CW_E::RenderWrapper render) const override
        {
            render.setView(m_Camera.getView());
            if (showShape)
                render.draw(shape);

            for (auto b : m_Beacons)
            {
                if (b)
                    b->draw(render);
            }
        }

        void onClosed() override
        {
            close();
        }

        void onKeyPressed(const sf::Event::KeyPressed* event) override
        {
            CW_MSG("KeyPressed Event happened in class MyApp.");
            if (event->code == sf::Keyboard::Key::Space)
            {
                CW_MSG("Pressed space.");
            }
        }

        void onMouseButtonPressed(const sf::Event::MouseButtonPressed* e) override
        {
            if (!m_Hold && e->button == sf::Mouse::Button::Left)
            {
                for (size_t i = 0; i < m_Beacons.size(); i++)
                {
                    if (!m_Beacons[i])
                    {
                        m_Beacons[i] = new Beacon{ i, m_Camera.worldPosition(e->position), getUpdateHandler(), getEventHandler() };
                        m_Hold = true;
                        return;
                    }
                }
                m_Beacons.emplace_back(new Beacon{ m_Beacons.size(), m_Camera.worldPosition(e->position), getUpdateHandler(), getEventHandler()});
                m_Hold = true;
            }
        }

        void onMouseButtonReleased(const sf::Event::MouseButtonReleased* e) override
        {
            m_Hold = m_Hold && e->button != sf::Mouse::Button::Left;
        }

        void onBeaconDischarge(const BeaconDischarge* e)
        {
            delete m_Beacons[e->index];
            m_Beacons[e->index] = nullptr;
        }

    private:
        float radius;
        sf::CircleShape shape;
        bool showShape = true;
        int counter = 0;

        Camera2D m_Camera;

        std::vector<Beacon*> m_Beacons;

        bool m_Hold = false;
    };

} // CW

std::unique_ptr<CW_E::Application> create_program(int argc, const char** argv, CW_E::EventHandlerWrapper eh, CW_E::UpdateHandlerWrapper uh)
{
    return std::make_unique<CW::MyApp>(eh, uh);
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
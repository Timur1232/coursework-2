#ifdef CW_TEST

#if 1
#include <engine/EntryPoint.h>

#include <engine/ProgramCore.h>
#include <imgui.h>
#include <imgui-SFML.h>

#include "debug_utils/Log.h"

#include "Camera2D.h"


class MyApp
    : public CW::Application,
      public CW::OnKeyPressed,
      public CW::OnClosed,
      public MyEventReciever,
      public OnThing
{
public:
    MyApp(CW::EventHandlerWrapper eventHandler, CW::UpdateHandlerWrapper updateHandler)
        : Application(800, 600, "test", eventHandler, updateHandler),
        radius(100.0f), shape(radius),
        m_Camera(0, 0, 800, 600, eventHandler, updateHandler)
    {
        shape.setFillColor(sf::Color::Green);
        eventHandler.subscribe(this);
    }

    void update() override
    {
        ImGui::Begin("Debug");
        ImGui::Checkbox("Show shape", &showShape);
        if (showShape)
        {
            ImGui::SliderFloat("radius", &radius, 10.0f, 200.0f);
            shape.setRadius(radius);
        }
        if (ImGui::Button("send event"))
        {
            getEventHandler().addEvent<EventData>(EventData{ 1 });
            getEventHandler().addEvent<EventData>(EventData{ 2 });
            getEventHandler().addEvent<EventData>(EventData{ 3 });
            getEventHandler().addEvent<EventData>(EventData{ 4 });
            getEventHandler().addEvent<EventData>(EventData{ 5 });
        }
        if (ImGui::Button("send other event"))
        {
            getEventHandler().addEvent<OtherEventData>(OtherEventData{ 10.5f, 6 });
            getEventHandler().addEvent<OtherEventData>(OtherEventData{ -4.6f, 23 });
        }
        if (ImGui::Button("send thing"))
        {
            getEventHandler().addEvent<Thing>(Thing{});
        }
        ImGui::End();
    }

    void draw(CW::RenderWrapper render) const override
    {
        render.setView(m_Camera.getView());
        if (showShape)
            render.draw(shape);
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

    void onE(const EventData* e) override
    {
        CW_MSG("Catched my own event! a = {}", e->a);
    }

    void gimme() override
    {
        CW_MSG("eeeeeeeeeeeeeeeeee");
    }

private:
    float radius;
    sf::CircleShape shape;
    bool showShape = true;
    int counter = 0;

    Camera2D m_Camera;
};

std::unique_ptr<CW::Application> create_program(int argc, const char** argv, CW::EventHandlerWrapper eh, CW::UpdateHandlerWrapper uh)
{
    return std::make_unique<MyApp>(eh, uh);
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
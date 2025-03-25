#ifdef CW_TEST

#if 0
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "debug/Log.h"
#include "debug/Profiler.h"

int main() {
    CW::Profiler::get().startSession();
    CW_PROFILE_FUNCTION();

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

    while (window.isOpen()) {
        CW_PROFILE_SCOPE("main loop");
        while (const std::optional event = window.pollEvent()) {
            CW_PROFILE_SCOPE("event loop");
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
                break;
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Hello, world!");
        ImGui::Text("This is a Dear ImGui window!");
        ImGui::Checkbox("Show shape", &showShape);
        if (showShape)
        {
            ImGui::SliderFloat("radius", &radius, 10.0f, 200.0f);
            shape.setRadius(radius);
        }
        ImGui::End();

        ImGui::Begin("Second window");                          // Create a window called "Hello, world!" and append into it.
        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        window.clear();

        if (showShape)
            window.draw(shape);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}

#else

#include <engine/EntryPoint.h>

#include <engine/ProgramCore.h>
#include <imgui.h>
#include <imgui-SFML.h>

class MyApp
    : public CW::Application
{
public:
    MyApp()
        : Application(800, 600, "hui"),
          radius(100.0f), shape(radius)
    {
        shape.setFillColor(sf::Color::Green);
    }

    void update() override
    {
        ImGui::Begin("Hello, world!");
        ImGui::Text("This is a Dear ImGui window!");
        ImGui::Checkbox("Show shape", &showShape);
        if (showShape)
        {
            ImGui::SliderFloat("radius", &radius, 10.0f, 200.0f);
            shape.setRadius(radius);
        }
        ImGui::End();

        ImGui::Begin("Second window");
        ImGui::Text("This is some useful text.");
        if (ImGui::Button("Button"))
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        ImGui::End();
    }

    void draw(CW::RenderWrapper renderwindow) override
    {
        if (showShape)
            renderwindow.draw(shape);
    }

    void onEvent(const sf::Event& event) override
    {
        if (event.is<sf::Event::Closed>())
            close();
    }

    void eventSubscribtion(CW::EventHandlerWrapper handler) override
    {
        handler.subscribe(this);
    }

private:
    float radius;
    sf::CircleShape shape;
    bool showShape = true;
    int counter = 0;
};

std::unique_ptr<CW::Application> create_program(int argc, const char** argv)
{
    return std::make_unique<MyApp>();
}

#endif

#endif
#ifndef CW_TEST

#if 0
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML + Dear ImGui");
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
        while (const std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
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

    window.close();
    ImGui::SFML::Shutdown();

    return 0;
}

#else

#include "debug/Log.h"
#include "debug/Profiler.h"

void foo1()
{
    CW_PROFILE_FUNCTION();

    std::cout << "Hello\n";
}

void foo2()
{
    CW_PROFILE_FUNCTION();
    int a = 5;
    a += 10;
    CW_TRACE("a is {}", a);
}

int main()
{
    CW::Profiler::get().startSession();

    CW_PROFILE_FUNCTION();

    foo1();
    foo2();

    /*CW_TRACE("Hello, world! {}", 5);
    CW_INFO("Hello, world! {}", 5);
    CW_WARN("Hello, world! {}", 5);
    CW_ERROR("Hello, world! {}", 5);
    CW_CRITICAL("Hello, world! {}", 5);*/
}

#endif

#endif
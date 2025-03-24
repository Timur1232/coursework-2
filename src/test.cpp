#ifdef CW_TEST

#include <SFML/Graphics.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

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

#endif
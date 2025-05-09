#include "pch.h"
#include "Renderer.h"

namespace CW {

    std::mutex WINDOW_MUTEX;
    sf::RenderWindow WINDOW;

    Renderer::Renderer(int width, int height, const char* title)
        : m_WindowSize(width, height), m_WindowTitle(title)
    {
        std::lock_guard<std::mutex> lock(WINDOW_MUTEX);
        WINDOW.create(sf::VideoMode(m_WindowSize), m_WindowTitle);
        //m_Window.setVerticalSyncEnabled(true);
        //m_Window.setFramerateLimit(60);

        /*if (!ImGui::SFML::Init(WINDOW))
        {
            CW_CRITICAL("Failing initializing ImGui::SFML.");
        }*/
    }

    Renderer::~Renderer()
    {
        CloseWindow();
    }

    void Renderer::CopyState(Application& app)
    {
        /*while (app.GetStatus() == AppStatus::SwappingBuffers);
        app.CollectState(*this);*/

        app.RequestCopy();
        while (app.GetStatus() != AppStatus::ReadyToCopy);
        app.CollectState(*this);
    }

    void Renderer::RecreateWindow(sf::State state)
    {
        std::lock_guard<std::mutex> lock(WINDOW_MUTEX);
        WINDOW.create(sf::VideoMode(m_WindowSize), m_WindowTitle, state);
    }

    void Renderer::CloseWindow()
    {
        std::lock_guard<std::mutex> lock(WINDOW_MUTEX);
        if (WINDOW.isOpen())
            WINDOW.close();
    }

} // CW
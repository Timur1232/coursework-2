#pragma once

#include "pch.h"
#include "engine/Application.h"

namespace CW {

    extern std::mutex WINDOW_MUTEX;
    extern sf::RenderWindow WINDOW;

    class Renderer
    {
    public:
        Renderer() = delete;
        Renderer(int width, int height, const char* title);
        virtual ~Renderer();

        virtual void Render(sf::Time deltaTime) = 0;
        void CopyState(Application& app);

        sf::Vector2u GetWindowSize() const { return m_WindowSize; }
        const char* GetTitle() const { return m_WindowTitle; }

        //sf::RenderWindow& GetWindow() { return m_Window; }
        void RecreateWindow(sf::State state);
        void CloseWindow();

    protected:
        sf::Vector2u m_WindowSize;
        const char* m_WindowTitle;
        //sf::RenderWindow m_Window;
    };

} // CW
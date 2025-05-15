#pragma once

#include "pch.h"

namespace CW {

    class Renderer
    {
    public:
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;

        static Renderer& Get();
        void Render(sf::RenderTarget& renderTarget);

        template <class... Args>
        Shared<sf::Texture> LoadTexture(Args&&... args)
        {
            return m_Textures.emplace_back(CreateShared<sf::Texture>(std::forward<Args>(args)...));
        }

        template <std::derived_from<sf::Drawable> T, class... Args>
        Shared<T> CreateResource(Args&&... args)
        {
            return m_GraphicalResources.emplace_back(CreateShared<T>(std::forward<Args>(args)...));
        }



    private:
        Renderer() = default;

    private:
        std::vector<Shared<sf::Texture>> m_Textures;
        std::vector<Shared<sf::Drawable>> m_GraphicalResources;
    };

} // CW
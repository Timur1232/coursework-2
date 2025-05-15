#include "pch.h"
#include "Renderer.h"

namespace CW {

    Renderer& Renderer::Get()
    {
        static Renderer renderer;
        return renderer;
    }

    void Renderer::Render(sf::RenderTarget& renderTarget)
    {
        for (const auto& resource : m_GraphicalResources)
        {
            renderTarget.draw(*resource);
        }
    }

} // CW
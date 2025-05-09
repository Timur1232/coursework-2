#pragma once

#include "pch.h"

namespace CW {

    class Renderer
    {
    public:
        Renderer() = default;
        virtual ~Renderer() = default;

        virtual void Render() const;

    private:

    };

} // CW
#pragma once

#include "pch.h"

namespace CW {

    template <class T>
    class DoubleBuffer
    {
    public:
        DoubleBuffer() = default;

        template <class... _Args>
        DoubleBuffer(_Args&&... args)
            : m_Buffers(std::forward<_Args>(args)...)
        {
        }

        const T& getBack() const { return m_Buffers.at(1 - m_CurrentBuffer); }
        T& getFront() { return m_Buffers[m_CurrentBuffer]; }

        T& operator*() { return m_Buffers[m_CurrentBuffer]; }
        const T& operator*() const { return m_Buffers.at(m_CurrentBuffer); }
        T* operator->() { return &m_Buffers[m_CurrentBuffer]; }
        const T* operator->() const { return &m_Buffers.at(m_CurrentBuffer); }

        void Swap() { m_CurrentBuffer = 1 - m_CurrentBuffer; }

    private:
        std::array<T, 2> m_Buffers;
        size_t m_CurrentBuffer = 0;
    };

} // CW
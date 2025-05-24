#pragma once

#include "pch.h"

namespace CW {

    class UPSLimiter
    {
    public:
        UPSLimiter();
        UPSLimiter(size_t ups);

        void Wait() const;
        void SetUPS(size_t ups);
        void Reset();

    private:
        std::chrono::microseconds m_LimitTime;
        mutable std::chrono::steady_clock::time_point m_NextPoint;
    };

} // CW
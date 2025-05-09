#pragma once

#include "pch.h"

namespace CW {

    class UPSLimiter
    {
    public:
        UPSLimiter(std::chrono::milliseconds limitTime);
        ~UPSLimiter();

    private:
        std::chrono::steady_clock::time_point m_StartPoint;
        std::chrono::milliseconds m_LimitTime;
    };

} // CW
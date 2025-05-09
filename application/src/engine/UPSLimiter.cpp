#include "pch.h"
#include "UPSLimiter.h"

namespace CW {

    UPSLimiter::UPSLimiter(std::chrono::milliseconds limitTime)
        : m_StartPoint(std::chrono::high_resolution_clock::now()),
          m_LimitTime(limitTime)
    {
    }

    UPSLimiter::~UPSLimiter()
    {
        auto endPoint = std::chrono::high_resolution_clock::now();
        auto elapsedTime = endPoint - m_StartPoint;
        auto remainingTime = m_LimitTime - elapsedTime;
        if (remainingTime.count() > 0)
        {
            std::this_thread::sleep_for(remainingTime);
        }
    }

} // CW
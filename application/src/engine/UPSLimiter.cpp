#include "pch.h"
#include "UPSLimiter.h"

namespace CW {

    UPSLimiter::UPSLimiter()
        : UPSLimiter(60)
    {
    }

    UPSLimiter::UPSLimiter(size_t ups)
        : m_LimitTime(1'000'000 / ups)
    {
        Reset();
    }

    void UPSLimiter::Wait()
    {
        auto now = std::chrono::high_resolution_clock::now();
        if (now < m_NextPoint)
        {
            std::this_thread::sleep_until(m_NextPoint);
        }
        m_NextPoint += m_LimitTime;
    }

    void UPSLimiter::SetUPS(size_t ups)
    {
        m_LimitTime = std::chrono::microseconds(1'000'000 / ups);
    }

    void UPSLimiter::Reset()
    {
        m_NextPoint = std::chrono::high_resolution_clock::now() + m_LimitTime;
    }

} // CW
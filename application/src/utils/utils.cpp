#include "pch.h"
#include "utils.h"

namespace CW {

    namespace angle {

        // ¬ычисл€ет четверть, в которой лежит угол
        // √раничные углы лежат в:
        //   0    -> I четверть
        //   90   -> II четверть
        //   -90  -> III четверть
        //   -180 -> IV четверть
        Quarter quarter(sf::Angle angle)
        {
            float fAngle = angle.wrapSigned().asRadians();

            if (fAngle >= 0.0f && fAngle < PI_2)
                return Quarter::First;
            if (fAngle >= PI_2 && fAngle < PI)
                return Quarter::Second;
            if (fAngle >= -PI && fAngle < -PI_2)
                return Quarter::Third;
            return Quarter::Fourth;
        }

    }

    sf::Vector2f ONE_LENGTH_VEC = { 1.0f, 0.0f };

} // CW

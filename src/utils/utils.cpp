#include "utils.h"

namespace CW {

    namespace angle {

        Quarter quarter(sf::Angle angle)
        {
            float fAngle = angle.wrapSigned().asRadians();

            if (fAngle >= 0.0f && fAngle < HALF_PI)
                return Quarter::First;
            if (fAngle >= HALF_PI && fAngle <= PI)
                return Quarter::Second;
            if (fAngle >= -PI && fAngle < -HALF_PI)
                return Quarter::Third;
            return Quarter::Fourth;
        }

    }

} // CW

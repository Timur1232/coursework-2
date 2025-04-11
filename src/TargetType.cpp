#include "TargetType.h"

namespace CW {

    TargetType opposite_target_type(TargetType type)
    {
        switch (type)
        {
        case TargetType::Navigation:    return TargetType::Recource;
        case TargetType::Recource:      return TargetType::Navigation;
        default:                        return TargetType::None;
        }
    }

} // CW
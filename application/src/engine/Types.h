#pragma once

#include <cstdint>

namespace CW {

    template <class T>
    using Unique = std::unique_ptr<T>;

    template <class T, class... Args>
    constexpr Unique<T> CreateUnique(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }


    template <class T>
    using Shared = std::shared_ptr<T>;

    template <class T, class... Args>
    constexpr Shared<T> CreateShared(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }


    using byte = std::uint8_t;

    using i8 = std::int8_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using u8 = std::uint8_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using f32 = float;
    using f64 = double;
    using f128 = long double;


    template <typename T>
    concept numeric = std::integral<T> || std::floating_point<T>;

} // CW
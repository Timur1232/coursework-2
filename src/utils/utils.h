#pragma once

#include <utils/ArenaAllocator.h>

namespace CW {

	template<class T>
	T clamp(T a, T min, T max)
	{
		return (a < min) ? min : (a > max) ? max : a;
	}

	template<class T>
	T loop(T a, T delta, T min, T max)
	{
		if (a + delta < min) a = max + (delta - a + min);
		else if (a + delta > max) a = min + (delta - max + a);
		else a += delta;
		return a;
	}

	template<class T>
	inline T lerp(T min, T max, double t)
	{
		return (max - min) * t + min;
	}

	constexpr uint64_t bit(unsigned int n)
	{
		return 1uLL << n;
	}

} // CW
#pragma once

#include <utils/ArenaAllocator.h>

namespace CW {

	template<class T>
	T clamp(T a, T min, T max)
	{
		if (a < min) a = min;
		else if (a > max) a = max;
		return a;
	}

	template<class T>
	T loop(T a, T delta, T min, T max)
	{
		if (a + delta < min) a = max + (delta - a + min);
		else if (a + delta > max) a = min + (delta - max + a);
		else a += delta;
		return a;
	}

	constexpr uint64_t bit(int n)
	{
		return 1uLL << n;
	}

} // CW
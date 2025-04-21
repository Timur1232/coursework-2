#pragma once

#include "pch.h"

namespace CW {

	/*template <class T>
	T clamp(T a, T min, T max)
	{
		return (a < min) ? min : (a > max) ? max : a;
	}*/

	template <class T>
	T loop(T a, T min, T max, T delta)
	{
		if (a + delta < min) a = max + (delta - a + min);
		else if (a + delta > max) a = min + (delta - max + a);
		else a += delta;
		return a;
	}

	template <class T>
	inline T lerp(T min, T max, float t)
	{
		return (max - min) * t + min;
	}

	constexpr uint64_t bit(unsigned int n)
	{
		return 1uLL << n;
	}

	template <class T>
	constexpr inline bool identical_signs(const T& a, const T& b)
	{
		return a > 0 && b > 0 || a < 0 && b < 0 || a == b;
	}

	template <class T>
	constexpr inline bool opposite_signs(const T& a, const T& b)
	{
		return a > 0 && b < 0 || a < 0 && b > 0;
	}


	namespace angle {

		constexpr inline float PI = sf::priv::pi;
		constexpr inline float HALF_PI = sf::priv::pi / 2.0f;

		enum class Quarter
		{
			First,
			Second,
			Third,
			Fourth
		};

		Quarter quarter(sf::Angle angle);

	} // angle

	template <class T>
	float distance(sf::Vector2<T> v1, sf::Vector2<T> v2)
	{
		return (v1 - v2).length();
	}

	template <class T>
	float distance_squared(sf::Vector2<T> v1, sf::Vector2<T> v2)
	{
		return (v1 - v2).lengthSquared();
	}

} // CW
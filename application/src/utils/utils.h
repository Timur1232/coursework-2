#pragma once

#include "pch.h"

namespace CW {

	template <class T>
	using Ref = std::reference_wrapper<T>;


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

	template <class T>
	constexpr inline T abs(T a)
	{
		return (a < 0) ? -a : a;
	}

	template <class T>
	constexpr inline bool in_between(const T& a, const T& alpha, const T& epsilon)
	{
		return a <= alpha + epsilon && a > alpha - epsilon;
	}

	template <class T>
	constexpr inline bool in_between_abs(const T& a, const T& alpha, const T& epsilon)
	{
		return abs(a) <= alpha + epsilon && abs(a) > alpha - epsilon;
	}


	namespace angle {

		constexpr inline float PI = sf::priv::pi;
		constexpr inline float PI_2 = sf::priv::pi / 2.0f;
		constexpr inline float PI_4 = sf::priv::pi / 4.0f;
		constexpr inline float PI_6 = sf::priv::pi / 6.0f;

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


	template <class T>
	struct Indexed
	{
		T Object;
		size_t Index = 0;

		Indexed() = default;

		template <class... Args_t>
		Indexed(Args_t&&... args)
			: Object(std::forward<Args_t>(args)...)
		{
		}

		Indexed(T&& other)
			: Object(std::forward<T>(other.Object)), Index(other.Index)
		{
		}

		T* operator->()
		{
			return &Object;
		}

		const T* operator->() const
		{
			return &Object;
		}
	};

} // CW
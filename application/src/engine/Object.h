#pragma once

#include "pch.h"

namespace CW {

	class Object
	{
	public:
		Object() = default;
		inline explicit Object(sf::Vector2f position)
			: m_Position(position)
		{
		}

		[[nodiscard]] inline sf::Vector2f GetPos() const { return m_Position; }
		inline void SetPos(sf::Vector2f position) { m_Position = position; }

	protected:
		sf::Vector2f m_Position;
	};


	template <class T>
	concept has_position = requires(T p, sf::Vector2f position)
	{
		{ p.GetPos() } -> std::same_as<sf::Vector2f>;
		{ p.SetPos(position) };
	};

} // CW
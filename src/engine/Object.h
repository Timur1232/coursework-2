#pragma once

#include "pch.h"

namespace CW {

	class Object
	{
	public:
		Object() = default;
		inline Object(sf::Vector2f position)
			: m_Position(position)
		{
		}

		[[nodiscard]] inline sf::Vector2f GetPos() const { return m_Position; }
		inline void SetPos(sf::Vector2f position) { m_Position = position; }

	protected:
		sf::Vector2f m_Position;
	};

} // CW
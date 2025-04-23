#include "pch.h"
#include "Chunks.h"

namespace CW {

	Chunk::Chunk(sf::Vector2i position, size_t reserve)
	{
		m_Objects.reserve(reserve);
		m_AdjacentChuncks.fill(nullptr);
	}

	Chunk::Chunk(sf::Vector2i position, Chunk&& other) noexcept
		: m_Objects(std::move(other.m_Objects)),
		  m_Position(position)
	{
	}

	const Chunk& Chunk::operator=(Chunk&& other) noexcept
	{
		m_Objects = std::move(other.m_Objects);
	}

	size_t Chunk::PushBack(Object* object)
	{
		m_Objects.push_back(object);
		return size_t();
	}

	void Chunk::PopBack()
	{
		m_Objects.pop_back();
	}

	void Chunk::Erase(size_t index)
	{
		m_Objects.erase(m_Objects.begin() + index);
	}

} // CW
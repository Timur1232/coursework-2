#include "pch.h"
#include "Chunks.h"

namespace CW {

	Chunk::Chunk(sf::Vector2i position, size_t reserve)
	{
		m_Objects.reserve(reserve);
		m_AdjacentChunks.fill(nullptr);
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

	Chunk::ObjectIterator Chunk::begin()
	{
		return m_Objects.begin();
	}

	Chunk::const_ObjectIterator Chunk::begin() const
	{
		return m_Objects.begin();
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
		std::swap(m_Objects[index], m_Objects.back());
		m_Objects.pop_back();
	}

	/*ChunkPackage::ChunkIterator::ChunkIterator(Chunk* chunk)
		: m_ChunkPtr(chunk)
	{
		if (chunk->Empty())
		{
			m_ChunkPtr = nullptr;
		}
	}

	ChunkPackage::ChunkIterator ChunkPackage::ChunkIterator::operator++()
	{
		if (!m_ChunkPtr)
			return *this;
		return ;
	}

	Object* ChunkPackage::ChunkIterator::operator*() const
	{
		if (m_ChunkPtr)
		{
			return *m_CurrentObject;
		}
		return nullptr;
	}*/

	const Chunk* ChunkHandler::GetChunk(sf::Vector2f requaredPosition)
	{
		sf::Vector2i flooredPosition = static_cast<sf::Vector2i>(requaredPosition);
		if (m_Chunks.contains(flooredPosition))
		{
			return &m_Chunks.at(flooredPosition);
		}
		return nullptr;
	}

	void ChunkHandler::AddObject(Object* object)
	{
		sf::Vector2i flooredPosition = static_cast<sf::Vector2i>(object->GetPos());
		m_Chunks[flooredPosition].PushBack(object);
		m_Size++;
	}

	size_t ChunkHandler::Size() const
	{
		return std::accumulate(m_Chunks.begin(), m_Chunks.end(), 0,
			[](const Chunk& chunk) { return chunk.Size(); });
	}

} // CW
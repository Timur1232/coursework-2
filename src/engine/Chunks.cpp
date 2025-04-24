#include "pch.h"
#include "Chunks.h"

namespace CW {

	/*Chunk::Chunk(sf::Vector2i position, size_t reserve)
	{
		m_Objects.reserve(reserve);
		m_AdjacentChunks.fill(nullptr);
	}

	Chunk::Chunk(sf::Vector2i position, Chunk&& other) noexcept
		: m_Objects(std::move(other.m_Objects)),
		  m_Index2D(position)
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
	}*/

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

	/*ChunkHandler::ChunkHandler(float chunckSize)
		: m_ChunkSize(chunckSize)
	{
	}

	const Chunk* ChunkHandler::GetChunk(sf::Vector2f requaredPosition)
	{
		sf::Vector2i chunkIndex = positionToChunkIndex(requaredPosition);
		if (m_Chunks.contains(chunkIndex))
		{
			return &m_Chunks.at(chunkIndex);
		}
		return nullptr;
	}

	size_t ChunkHandler::AddObject(Object* object)
	{
		sf::Vector2i chunkIndex = positionToChunkIndex(object->GetPos());
		return m_Chunks[chunkIndex].PushBack(object);
	}

	sf::Vector2i ChunkHandler::positionToChunkIndex(sf::Vector2f position) const
	{
		return sf::Vector2i(static_cast<int>(position.x / m_ChunkSize), static_cast<int>(position.y / m_ChunkSize));
	}*/

} // CW
#pragma once

#include "pch.h"

#include "engine/Object.h"
#include "debug_utils/Log.h"
#include "utils/utils.h"

namespace std {
	template <>
	struct hash<sf::Vector2i> {
		size_t operator()(const sf::Vector2i& v) const {
			return hash<int>()(v.x) ^ hash<int>()(v.y);
		}
	};
}

namespace CW {

	template <has_position T, size_t RESERVE = 32>
	class Chunk
	{
	public:
		using ObjectIterator = std::vector<Indexed<T, 2>*>::iterator;
		using const_ObjectIterator = std::vector<Indexed<T, 2>*>::const_iterator;

	public:
		Chunk(sf::Vector2i key)
			: m_Key(key)
		{
			m_Objects.reserve(RESERVE);
		}

		explicit Chunk(Chunk<T>&& other) noexcept
			: m_Objects(std::move(other.m_Objects)),
			  m_Key(other.m_Key),
			  m_DeadPtrs(other.m_DeadPtrs)
		{
			other.m_Key = { 0, 0 };
			other.m_DeadPtrs = 0;
		}

		[[nodiscard]] const Chunk<T>& operator=(Chunk<T>&& other) noexcept
		{
			m_Objects = std::move(other.m_Objects);
			m_Key = other.m_Key;
			m_DeadPtrs = other.m_DeadPtrs;
			other.m_Key = { 0, 0 };

			other.m_DeadPtrs = 0;
		}

		[[nodiscard]] bool Empty() const { return m_Objects.empty(); }
		[[nodiscard]] bool Size() const { return m_Objects.size(); }
		[[nodiscard]] bool Capacity() const { return m_Objects.capacity(); }
		void Clear() { m_Objects.clear(); }

		[[nodiscard]] const std::vector<Indexed<T, 2>*>& GetObjects() const { return m_Objects; }

		[[nodiscard]]  sf::Vector2i GetKey() const { return m_Key; }

		[[nodiscard]] ObjectIterator begin()
		{
			return m_Objects.begin();
		}

		[[nodiscard]] const_ObjectIterator begin() const
		{
			return m_Objects.begin();
		}

		[[nodiscard]] ObjectIterator end()
		{
			return m_Objects.end();
		}

		[[nodiscard]] const_ObjectIterator end() const
		{
			return m_Objects.end();
		}

		void PushBack(Indexed<T, 2>& object)
		{
			if (m_DeadPtrs)
			{
				m_Objects[m_Objects.size() - m_DeadPtrs] = &object;
				object.Indecies[1] = m_Objects.size() - m_DeadPtrs;
				--m_DeadPtrs;
			}
			else
			{
				m_Objects.push_back(&object);
				m_Objects.back()->Indecies[1] = m_Objects.size() - 1;
			}
		}

		void PopBack()
		{
			m_Objects.pop_back();
		}

		void ForgetObject(Indexed<T, 2>& object)
		{
			size_t index = object.Indecies[1];
			if (index >= m_Objects.size())
			{
				CW_ERROR("Non-valid object index in chunk! index: {}, size: {}, position: ({:.2f}, {:.2f})", index, m_Objects.size(), object->GetPos().x, object->GetPos().y);
				return;
			}
			m_Objects[index] = nullptr;
			if ((((i64) m_Objects.size() - (i64) m_DeadPtrs) > 2) && (index != (m_Objects.size() - m_DeadPtrs - 1)))
			{
				std::swap(m_Objects[index], m_Objects[m_Objects.size() - m_DeadPtrs - 1]);
				m_Objects[index]->Indecies[1] = index;
			}
			++m_DeadPtrs;
		}

	private:
		std::vector<Indexed<T, 2>*> m_Objects;
		size_t m_DeadPtrs = 0;
		sf::Vector2i m_Key;
	};


	template <has_position T>
	struct ChunkPtr
	{
		const Chunk<T>* CenterChunk = nullptr;
		std::array<const Chunk<T>*, 8> AdjacentChunks = nullptr;
	};


	template <has_position T>
	class ChunkHandler
	{
	public:
		ChunkHandler() = default;
		explicit ChunkHandler(float chunckSize)
			: m_ChunkSize(chunckSize)
		{
		}

		[[nodiscard]] ChunkPtr<T> GetAreaChunk(sf::Vector2f requaredPosition) const
		{
			ChunkPtr<T> chunk;
			sf::Vector2i chunkKey = positionToChunkKey(requaredPosition);
			if (m_ChunkIndecies.contains(chunkKey))
			{
				chunk.CenterChunk = &m_Chunks.at(m_ChunkIndecies.at(chunkKey));
			}

			size_t i = 0;
			for (int offsetX = -1; offsetX <= 1; ++offsetX)
			{
				for (int offsetY = -1; offsetY <= 1; ++offsetY)
				{
					if (offsetX == 0 && offsetY == 0)
						continue;
					sf::Vector2i offset = chunkKey + sf::Vector2i(offsetX, offsetY);
					if (m_ChunkIndecies.contains(offset))
					{
						chunk.AdjacentChunks[i] = &m_Chunks.at(m_ChunkIndecies.at(offset));
					}
					i++;
				}
			}

			return chunk;
		}

		void AddObject(Indexed<T, 2>& object)
		{
			sf::Vector2i chunkKey = positionToChunkKey(object->GetPos());
			size_t chunkIndex = 0;
			if (m_ChunkIndecies.contains(chunkKey))
			{
				chunkIndex = m_ChunkIndecies.at(chunkKey);
			}
			else
			{
				chunkIndex = m_Chunks.size();
				m_ChunkIndecies[chunkKey] = chunkIndex;
				m_Chunks.emplace_back(chunkKey);
			}
			m_Chunks[chunkIndex].PushBack(object);
			object.Indecies[0] = chunkIndex;
		}

		void ForgetObject(Indexed<T, 2>& object)
		{
			size_t chunkIndex = object.Indecies[0];
			m_Chunks[chunkIndex].ForgetObject(object);
		}

		[[nodiscard]] inline const std::vector<Chunk<T>>& GetAllChunks() const { return m_Chunks; }
		[[nodiscard]] size_t Size() const { return m_Chunks.size(); }

		void Clear()
		{
			for (auto& chunk : m_Chunks)
			{
				chunk.Clear();
			}
		}

		float GetChunkSize() const { return m_ChunkSize; }

	private:
		sf::Vector2i positionToChunkKey(sf::Vector2f position) const
		{
			sf::Vector2i key(static_cast<int>(position.x / m_ChunkSize), static_cast<int>(position.y / m_ChunkSize));
			if (position.x < 0)
				key.x -= 1;
			if (position.y < 0)
				key.y -= 1;
			return key;
		}

	private:
		std::unordered_map<sf::Vector2i, size_t> m_ChunkIndecies;
		std::vector<Chunk<T>> m_Chunks;
		const float m_ChunkSize = 500.0f;
	};

} // CW
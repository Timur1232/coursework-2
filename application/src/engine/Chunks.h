#pragma once

#include "pch.h"

#include "engine/Object.h"
#include "debug_utils/Log.h"

namespace std {
	template <>
	struct hash<sf::Vector2i> {
		size_t operator()(const sf::Vector2i& v) const {
			return hash<int>()(v.x) ^ hash<int>()(v.y);
		}
	};
}

namespace CW {

	/*class ChunkPackage
	{
	public:
		class ChunkIterator
		{
		public:
			using Self = ChunkIterator;

		public:
			ChunkIterator(Chunk* chunk);

			inline bool operator!=(const Self& other) const { return m_ChunkPtr != other.m_ChunkPtr; }
			Self operator++();
			Object* operator*() const;

		private:
			Chunk* m_ChunkPtr;
			int m_ChunkIndex = -1;
			Chunk::ObjectIterator m_CurrentObject;
		};

		class const_ChunkIterator
		{
		public:
			using Self = const_ChunkIterator;

		public:
			const_ChunkIterator(const Chunk* chunk);

			inline bool operator!=(const Self& other) const { return m_ChunkPtr != other.m_ChunkPtr; }
			Self operator++();
			inline const Object* operator*() const { return *m_CurrentObject; }

		private:
			const Chunk* m_ChunkPtr;
			int m_ChunkIndex = -1;
			Chunk::const_ObjectIterator m_CurrentObject;
		};

	public:
		ChunkPackage(const Chunk* chunk);

		ChunkIterator begin();
		ChunkIterator end();

		const_ChunkIterator begin() const;
		const_ChunkIterator end() const;

	private:
		const Chunk* const m_CenterChunk;
	};*/

	// TODO: сделать что-то с RESERVE
	template <std::derived_from<Object> T, size_t RESERVE = 32>
	class Chunk
	{
	public:
		using ObjectIterator = std::vector<T*>::iterator;
		using const_ObjectIterator = std::vector<T*>::const_iterator;

	public:
		Chunk()
		{
			m_Objects.reserve(RESERVE);
		}

		explicit Chunk(Chunk<T>&& other) noexcept
			: m_Objects(std::move(other.m_Objects))
		{
		}

		[[nodiscard]] const Chunk<T>& operator=(Chunk<T>&& other) noexcept
		{
			m_Objects = std::move(other.m_Objects);
		}

		[[nodiscard]] inline bool Empty() const { return m_Objects.empty(); }
		[[nodiscard]] inline bool Size() const { return m_Objects.size(); }
		[[nodiscard]] inline bool Capacity() const { return m_Objects.capacity(); }
		inline void Clear() { m_Objects.clear(); }

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

		[[nodiscard]] inline T& operator[](size_t index) { return *m_Objects[index]; }
		[[nodiscard]] inline const T& At(size_t index) const { return *m_Objects.at(index); }

		inline size_t PushBack(T* object)
		{
			if (m_DeadPtrs)
			{
				for (size_t i = 0; i < m_Objects.size(); ++i)
				{
					if (!m_Objects[i])
					{
						m_Objects[i] = object;
						--m_DeadPtrs;
						return i;
					}
				}
			}

			m_Objects.push_back(object);
			return m_Objects.size() - 1;
		}

		inline void PopBack()
		{
			m_Objects.pop_back();
		}

		inline void ForgetObject(size_t index)
		{
			m_Objects[index] = nullptr;
			++m_DeadPtrs;
		}

	private:
		std::vector<T*> m_Objects;
		size_t m_DeadPtrs = 0;
	};


	template <std::derived_from<Object> T>
	struct ChunkPtr
	{
		const Chunk<T>* CenterChunk = nullptr;
		std::array<const Chunk<T>*, 8> AdjacentChunks = nullptr;
	};


	template <std::derived_from<Object> T>
	class ChunkHandler
	{
	public:
		ChunkHandler() = default;
		explicit ChunkHandler(float chunckSize)
			: m_ChunkSize(chunckSize)
		{
		}

		[[nodiscard]] ChunkPtr<T> GetChunk(sf::Vector2f requaredPosition) const
		{
			ChunkPtr<T> chunk;
			sf::Vector2i chunkKey = positionToChunkKey(requaredPosition);
			if (m_Chunks.contains(chunkKey))
			{
				chunk.CenterChunk = &m_Chunks.at(chunkKey);
			}

			size_t i = 0;
			for (int offsetX = -1; offsetX <= 1; ++offsetX)
			{
				for (int offsetY = -1; offsetY <= 1; ++offsetY)
				{
					if (offsetX == 0 && offsetY == 0)
						continue;
					sf::Vector2i offset = chunkKey + sf::Vector2i(offsetX, offsetY);
					if (m_Chunks.contains(offset))
					{
						chunk.AdjacentChunks[i] = &m_Chunks.at(offset);
					}
					i++;
				}
			}

			return chunk;
		}

		[[nodiscard]] size_t AddObject(T* object)
		{
			sf::Vector2i chunkKey = positionToChunkKey(object->GetPos());
			return m_Chunks[chunkKey].PushBack(object);
		}

		void ForgetObject(sf::Vector2f objectPosition, size_t index)
		{
			sf::Vector2i chunkKey = positionToChunkKey(objectPosition);
			m_Chunks[chunkKey].ForgetObject(index);
		}

		[[nodiscard]] inline const std::unordered_map<sf::Vector2i, Chunk<T>>& GetAllChunks() const { return m_Chunks; }
		[[nodiscard]] inline size_t Size() const { return m_Chunks.size(); }

		void Clear()
		{
			for (auto& [key, chunk] : m_Chunks)
			{
				chunk.Clear();
			}
		}

	private:
		sf::Vector2i positionToChunkKey(sf::Vector2f position) const
		{
			sf::Vector2i key(static_cast<int>(position.x / m_ChunkSize), static_cast<int>(position.y / m_ChunkSize));
			if (position.x < 0)
				key.x--;
			if (position.y < 0)
				key.y--;
			return key;
		}

	private:
		std::unordered_map<sf::Vector2i, Chunk<T>> m_Chunks;
		float m_ChunkSize = 500.0f;
	};

} // CW
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


	template <class T>
	class Chunk
	{
	public:
		using ObjectIterator = std::vector<T*>::iterator;
		using const_ObjectIterator = std::vector<T*>::const_iterator;

	public:
		Chunk() = default;
		Chunk(sf::Vector2i position, size_t reserve = 32)
		{
			m_Objects.reserve(reserve);
		}

		explicit Chunk(sf::Vector2i position, Chunk&& other) noexcept
			: m_Objects(std::move(other.m_Objects)),
		  	  m_Index2D(position)
		{
		}

		[[nodiscard]] const Chunk& operator=(Chunk&& other) noexcept
		{
			m_Objects = std::move(other.m_Objects);
		}

		[[nodiscard]] inline sf::Vector2i GetIndex2D() const { return m_Index2D; }
		[[nodiscard]] inline void SetIndex2D(sf::Vector2i position) { m_Index2D = position; }

		[[nodiscard]] inline bool Empty() const { return m_Objects.empty(); }
		[[nodiscard]] inline bool Size() const { return m_Objects.size(); }
		[[nodiscard]] inline bool Capacity() const { return m_Objects.capacity(); }

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

		inline void Erase(size_t index)
		{
			/*std::swap(m_Objects[index], m_Objects.back());
			m_Objects.pop_back();*/
			m_Objects[index] = nullptr;
			++m_DeadPtrs;
		}

	private:
		sf::Vector2i m_Index2D;
		std::vector<T*> m_Objects;
		size_t m_DeadPtrs = 0;
	};

	template <class T>
	struct ChunkPtr
	{
		const Chunk<T>* CenterChunk = nullptr;
		std::array<const Chunk<T>*, 8> AdjacentChunks = nullptr;
	};

	template <class T>
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
			sf::Vector2i chunkIndex = positionToChunkIndex(requaredPosition);
			if (m_Chunks.contains(chunkIndex))
			{
				chunk.CenterChunk = &m_Chunks.at(chunkIndex);
			}

			size_t i = 0;
			for (int offsetX = -1; offsetX <= 1; ++offsetX)
			{
				for (int offsetY = -1; offsetY <= 1; ++offsetY)
				{
					if (offsetX == 0 && offsetY == 0)
						continue;
					sf::Vector2i offset = chunkIndex + sf::Vector2i(offsetX, offsetY);
					if (m_Chunks.contains(offset))
					{
						chunk.AdjacentChunks[i] = &m_Chunks.at(offset);
					}
					i++;
				}
			}

			return chunk;
		}

		size_t AddObject(T* object)
		{
			sf::Vector2i chunkIndex = positionToChunkIndex(object->GetPos());
			//CW_TRACE("Beacon added to chunk: ({}, {})", chunkIndex.x, chunkIndex.y);
			return m_Chunks[chunkIndex].PushBack(object);
		}

		void EraseObject(sf::Vector2f objectPosition, size_t index)
		{
			sf::Vector2i chunkIndex = positionToChunkIndex(objectPosition);
			m_Chunks[chunkIndex].Erase(index);
			//CW_TRACE("Beacon erased from chunk: ({}, {})", chunkIndex.x, chunkIndex.y);
		}

	private:
		sf::Vector2i positionToChunkIndex(sf::Vector2f position) const
		{
			return sf::Vector2i(static_cast<int>(position.x / m_ChunkSize), static_cast<int>(position.y / m_ChunkSize));
		}

	private:
		std::unordered_map<sf::Vector2i, Chunk<T>> m_Chunks;
		float m_ChunkSize = 10.0f;
	};

} // CW
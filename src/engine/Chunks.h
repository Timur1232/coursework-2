#pragma once

#include "pch.h"

#include "engine/Object.h"

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

	class Chunk
	{
	public:
		using ObjectIterator = std::vector<Object*>::iterator;
		using const_ObjectIterator = std::vector<Object*>::const_iterator;

	public:
		Chunk() = default;
		explicit Chunk(sf::Vector2i position, size_t reserve = 32);
		explicit Chunk(sf::Vector2i position, Chunk&& other) noexcept;

		[[nodiscard]] const Chunk& operator=(Chunk&& other) noexcept;

		[[nodiscard]] inline sf::Vector2i GetPos() const { return m_Position; }
		[[nodiscard]] inline void SetPos(sf::Vector2i position) { m_Position = position; }

		[[nodiscard]] inline const std::array<Chunk*, 8>& GetAdjacentChunks() const { return m_AdjacentChunks; }

		[[nodiscard]] inline bool Empty() const { return m_Objects.empty(); }
		[[nodiscard]] inline bool Size() const { return m_Objects.size(); }
		[[nodiscard]] inline bool Capacity() const { return m_Objects.capacity(); }

		[[nodiscard]] ObjectIterator begin();
		[[nodiscard]] const_ObjectIterator begin() const;

		[[nodiscard]] inline Object& operator[](size_t index) { return *m_Objects[index]; }
		[[nodiscard]] inline const Object& At(size_t index) const { return *m_Objects.at(index); }

		inline size_t PushBack(Object* object);
		inline void PopBack();
		inline void Erase(size_t index);

	private:
		sf::Vector2i m_Position;
		std::vector<Object*> m_Objects;
		std::array<Chunk*, 8> m_AdjacentChunks;
	};

	class ChunkHandler
	{
	public:
		ChunkHandler() = default;

		[[nodiscard]] const Chunk* GetChunk(sf::Vector2f requaredPosition);
		void AddObject(Object* object);

	private:
		std::unordered_map<sf::Vector2i, Chunk> m_Chunks;
	};

} // CW
#pragma once

#include "pch.h"

#include "engine/Object.h"

namespace CW {

	class Chunk
	{
	public:
		using ObjectIterator = std::vector<Object*>::iterator;
		using const_ObjectIterator = std::vector<Object*>::const_iterator;

	public:
		Chunk() = default;
		explicit Chunk(sf::Vector2i position, size_t reserve = 0);
		explicit Chunk(sf::Vector2i position, Chunk&& other) noexcept;

		[[nodiscard]] const Chunk& operator=(Chunk&& other) noexcept;

		[[nodiscard]] inline sf::Vector2i GetPos() const { return m_Position; }
		[[nodiscard]] inline void SetPos(sf::Vector2i position) { m_Position = position; }

		[[nodiscard]] inline bool Empty() const { return m_Objects.empty(); }
		[[nodiscard]] inline bool Size() const { return m_Objects.size(); }
		[[nodiscard]] inline bool Capacity() const { return m_Objects.capacity(); }

		[[nodiscard]] ObjectIterator GetObjectIter();
		[[nodiscard]] const_ObjectIterator GetObjectIter() const;

		[[nodiscard]] inline Object& operator[](size_t index) { return *m_Objects[index]; }
		[[nodiscard]] inline const Object& At(size_t index) const { return *m_Objects.at(index); }

		inline size_t PushBack(Object* object);
		inline void PopBack();
		inline void Erase(size_t index);

	private:
		sf::Vector2i m_Position;
		std::vector<Object*> m_Objects;
		std::array<Chunk*, 8> m_AdjacentChuncks;
	};

	class ChunkHandler
	{
	public:
		ChunkHandler(size_t reserve);

		void AddObject(Object* object);

		[[nodiscard]] Chunk& GetChunk(sf::Vector2f requaredPosition);

	private:
		std::vector<Chunk> m_Chuncks;
	};

} // CW
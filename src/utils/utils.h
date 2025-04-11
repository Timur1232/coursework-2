#pragma once

#include <cstdint>
#include <exception>
#include <SFML/System/Angle.hpp>

namespace CW {

	/*template <class T>
	T clamp(T a, T min, T max)
	{
		return (a < min) ? min : (a > max) ? max : a;
	}*/

	template <class T>
	T loop(T a, T min, T max, T delta)
	{
		if (a + delta < min) a = max + (delta - a + min);
		else if (a + delta > max) a = min + (delta - max + a);
		else a += delta;
		return a;
	}

	template <class T>
	inline T lerp(T min, T max, float t)
	{
		return (max - min) * t + min;
	}

	constexpr uint64_t bit(unsigned int n)
	{
		return 1uLL << n;
	}

	template <class T>
	constexpr inline bool identical_signs(const T& a, const T& b)
	{
		return a > 0 && b > 0 || a < 0 && b < 0 || a == b;
	}

	template <class T>
	constexpr inline bool opposite_signs(const T& a, const T& b)
	{
		return a > 0 && b < 0 || a < 0 && b > 0;
	}


	namespace angle {

		constexpr inline float PI = sf::priv::pi;
		constexpr inline float HALF_PI = sf::priv::pi / 2.0f;

		enum class Quarter
		{
			First,
			Second,
			Third,
			Fourth
		};

		Quarter quarter(sf::Angle angle);

	} // angle



	template <class T>
	struct MemoryBlock
	{
		size_t allocated;
		size_t capasity;
		MemoryBlock<T>* next;
		T* memory;

		MemoryBlock(size_t size)
			: allocated(0), capasity(size), next(nullptr)
		{
			memory = new T[size];
			if (!memory)
			{
				throw std::bad_alloc();
			}
		}

		~MemoryBlock()
		{
			delete[] memory;
		}

		T* allocate(size_t count = 1)
		{
			T* mem = &memory[allocated];
			allocated += count;
			return mem;
		}
	};


	template <class T>
	class ArenaAllocator
	{
	public:
		ArenaAllocator(size_t blockSize = 1024)
			: m_BlockSize(blockSize)
		{
			m_BlockList = new MemoryBlock<T>(m_BlockSize);
			if (!m_BlockList)
			{
				throw std::bad_alloc();
			}
			m_CurrentBlock = m_BlockList;
		}
		~ArenaAllocator()
		{
			free();
		}

		T* allocate(size_t count = 1)
		{
			if (m_CurrentBlock->capasity - m_CurrentBlock->allocated < count)
			{
				newBlock();
			}
			return m_CurrentBlock->allocate(count);
		}

		void free()
		{
			MemoryBlock<T>* tmp1 = m_BlockList;
			MemoryBlock<T>* tmp2 = m_BlockList->next;

			while (tmp2 != nullptr)
			{
				delete tmp1;
				tmp1 = tmp2;
				tmp2 = tmp2->next;
			}
			delete tmp1;
		}

		void deallocate()
		{
			m_CurrentBlock = m_BlockList;
			MemoryBlock<T>* iter = m_BlockList;
			while (iter != nullptr)
			{
				iter->allocated = 0;
				iter = iter->next;
			}
		}

	private:
		void newBlock()
		{
			if (m_CurrentBlock->next)
			{
				m_CurrentBlock = m_CurrentBlock->next;
				return;
			}

			m_CurrentBlock->next = new MemoryBlock<T>(m_BlockSize);
			if (!m_CurrentBlock->next)
			{
				throw std::bad_alloc();
			}
			m_CurrentBlock = m_CurrentBlock->next;
		}

	private:
		MemoryBlock<T>* m_BlockList;
		MemoryBlock<T>* m_CurrentBlock;
		size_t m_BlockSize = 1024;
	};

} // CW
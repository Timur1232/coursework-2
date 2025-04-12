#pragma once

#include <exception>

namespace CW {

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
			: m_BlockSize(blockSize), m_TotalCapasity(blockSize)
		{
			m_BlockList = new MemoryBlock<T>(m_BlockSize);
			if (!m_BlockList)
			{
				throw std::bad_alloc();
			}
			m_CurrentBlock = m_BlockList;
			m_BlockCount = 1;
		}
		~ArenaAllocator()
		{
			free();
		}

		inline size_t currentAllocated() const
		{
			return m_CurrentBlock->allocated * sizeof(T);
		}

		inline size_t currentCapasity() const
		{
			return m_CurrentBlock->capasity * sizeof(T);
		}

		inline size_t totalAllocated() const
		{
			return m_TotalAllocated * sizeof(T);
		}

		inline size_t totalCapasity() const
		{
			return m_TotalCapasity * sizeof(T);
		}

		inline size_t blockCount() const
		{
			return m_BlockCount;
		}

		T* allocate(size_t count = 1)
		{
			if (!m_CurrentBlock || m_CurrentBlock->capasity - m_CurrentBlock->allocated < count)
			{
				newBlock();
			}
			m_TotalAllocated += count;
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

			m_CurrentBlock = m_BlockList = nullptr;

			m_TotalAllocated = 0;
			m_TotalCapasity = 0;
			m_BlockCount = 0;
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
			m_TotalAllocated = 0;
		}

	private:
		void newBlock()
		{
			if (m_CurrentBlock && m_CurrentBlock->next)
			{
				m_CurrentBlock = m_CurrentBlock->next;
				return;
			}

			if (m_CurrentBlock)
			{
				m_CurrentBlock->next = new MemoryBlock<T>(m_BlockSize);
				if (!m_CurrentBlock->next)
				{
					throw std::bad_alloc();
				}
				m_CurrentBlock = m_CurrentBlock->next;
			}
			else
			{
				m_CurrentBlock = new MemoryBlock<T>(m_BlockSize);
			}

			m_TotalCapasity += m_BlockSize;
			++m_BlockCount;
		}

	private:
		MemoryBlock<T>* m_BlockList;
		MemoryBlock<T>* m_CurrentBlock;
		size_t m_BlockSize;

		// Debug
		size_t m_TotalAllocated = 0;
		size_t m_TotalCapasity;
		size_t m_BlockCount;
	};

} // CW
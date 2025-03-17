#pragma once

#include <exception>
#include <iostream>

namespace CW
{
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
			std::cout << "Created memory block: " << size * sizeof(T) << " bytes" << std::endl;
		}

		~MemoryBlock()
		{
			delete[] memory;
			std::cout << "Deleted memory block: " << capasity * sizeof(T) << " bytes" << std::endl;
		}

		T* allocate(size_t count = 1)
		{
			T* mem = &memory[allocated];
			allocated += count;
			std::cout << "Allocated " << count * sizeof(T) << " bytes" << std::endl;
			return mem;
		}
	};

	template <class T, size_t BLOCK_SIZE = 1024>
	class ArenaAllocator
	{
	public:
		ArenaAllocator()
		{
			m_BlockList = new MemoryBlock<T>(BLOCK_SIZE);
			if (!m_BlockList)
			{
				throw std::bad_alloc();
			}
			m_CurrentBlock = m_BlockList;
		}
		~ArenaAllocator()
		{
			dealocate();
		}

		T* allocate(size_t count = 1)
		{
			if (m_CurrentBlock->capasity - m_CurrentBlock->allocated < count)
			{
				newBlock();
			}
			return m_CurrentBlock->allocate(count);
		}

		void dealocate()
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

	private:
		void newBlock()
		{
			m_CurrentBlock->next = new MemoryBlock<T>(BLOCK_SIZE);
			if (!m_CurrentBlock->next)
			{
				throw std::bad_alloc();
			}
			m_CurrentBlock = m_CurrentBlock->next;
		}

	private:
		MemoryBlock<T>* m_BlockList;
		MemoryBlock<T>* m_CurrentBlock;
	};

}
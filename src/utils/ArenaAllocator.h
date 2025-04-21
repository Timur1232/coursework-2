#pragma once

#include "pch.h"

namespace CW {

	template <class T>
	struct MemoryBlock
	{
		size_t Allocated;
		size_t Capasity;
		MemoryBlock<T>* Next;
		T* Memory;

		MemoryBlock(size_t size)
			: Allocated(0), Capasity(size), Next(nullptr)
		{
			Memory = new T[size];
			if (!Memory)
			{
				throw std::bad_alloc();
			}
		}

		~MemoryBlock()
		{
			delete[] Memory;
		}

		T* Allocate(size_t count = 1)
		{
			T* mem = &Memory[Allocated];
			Allocated += count;
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
			Free();
		}

		inline size_t CurrentAllocated() const
		{
			return m_CurrentBlock->Allocated * sizeof(T);
		}

		inline size_t CurrentCapasity() const
		{
			return m_CurrentBlock->Capasity * sizeof(T);
		}

		inline size_t TotalAllocated() const
		{
			return m_TotalAllocated * sizeof(T);
		}

		inline size_t TotalCapasity() const
		{
			return m_TotalCapasity * sizeof(T);
		}

		inline size_t BlockCount() const
		{
			return m_BlockCount;
		}

		T* Allocate(size_t count = 1)
		{
			if (!m_CurrentBlock || m_CurrentBlock->Capasity - m_CurrentBlock->Allocated < count)
			{
				newBlock();
			}
			m_TotalAllocated += count;
			return m_CurrentBlock->Allocate(count);
		}

		void Free()
		{
			MemoryBlock<T>* tmp1 = m_BlockList;
			MemoryBlock<T>* tmp2 = m_BlockList->Next;

			while (tmp2 != nullptr)
			{
				delete tmp1;
				tmp1 = tmp2;
				tmp2 = tmp2->Next;
			}
			delete tmp1;

			m_CurrentBlock = m_BlockList = nullptr;

			m_TotalAllocated = 0;
			m_TotalCapasity = 0;
			m_BlockCount = 0;
		}

		void Deallocate()
		{
			m_CurrentBlock = m_BlockList;
			MemoryBlock<T>* iter = m_BlockList;
			while (iter != nullptr)
			{
				iter->Allocated = 0;
				iter = iter->Next;
			}
			m_TotalAllocated = 0;
		}

	private:
		void newBlock()
		{
			if (m_CurrentBlock && m_CurrentBlock->Next)
			{
				m_CurrentBlock = m_CurrentBlock->Next;
				return;
			}

			if (m_CurrentBlock)
			{
				m_CurrentBlock->Next = new MemoryBlock<T>(m_BlockSize);
				if (!m_CurrentBlock->Next)
				{
					throw std::bad_alloc();
				}
				m_CurrentBlock = m_CurrentBlock->Next;
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
#pragma once

#include "utils/utils.h"
#include <vector>

namespace CW_E {

	// TODO
	template<class T, class Ptr_T = T>
	class PtrIndxArray
	{
	public:


	private:
		ArenaAllocator<T> m_Allocator;
		std::vector<Ptr_T*> m_Ptrs;
	};

} // CW_E
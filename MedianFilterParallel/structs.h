#pragma once
#include <cstddef>

namespace hpc
{
	template < typename T >
	struct array
	{
		int size;
		T* pointer;

		array(int s, T* p) : size(s), pointer(p) {}
		array() {}
	};
}
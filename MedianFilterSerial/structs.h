#pragma once
namespace hpc
{
	template < typename T >
	struct array
	{
		size_t size;
		T* pointer;

		array(size_t s, T* p) : size(s), pointer(p) {}
		array() {}
	};
}
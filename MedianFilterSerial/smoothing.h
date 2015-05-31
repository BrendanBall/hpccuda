#pragma once
#include "structs.h"

namespace hpcserial
{
	class smoothing
	{
	private:
		int* bins;
		int* window;
		size_t resolution;
		size_t binsize;
		size_t filtersize;
	public:
		smoothing(size_t resolution, size_t binsize, int* bins, size_t filtersize);

		void applyFilter();

		int hpcserial::smoothing::median(int);

		void hpcserial::smoothing::sorttest();

		~smoothing();
	};

}


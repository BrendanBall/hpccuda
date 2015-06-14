#pragma once
#include "structs.h"

namespace hpcparallel
{
	class smoothing
	{
	private:
		size_t resolution;
		size_t binsize;
		int* bins;
		size_t filtersize;
		int* window;
	public:
		smoothing(size_t resolution, size_t binsize, int* bins, size_t filtersize);

		void applyFilter();

		int median(int currentBin);

		void sorttest();

		~smoothing();
	};

}


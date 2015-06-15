#pragma once
#include "structs.h"

namespace hpcparallel
{
	class smoothing
	{
	private:
		int resolution;
		int binsize;
		int* bins;
		int filtersize;
		int* window;
	public:
		smoothing(int resolution, int binsize, int* bins, int filtersize);

		void applyFilter();

		int median(int currentBin);

		void sorttest();

		~smoothing();
	};

}


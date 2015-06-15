#pragma once
#include "structs.h"

namespace hpcserial
{
	class smoothing
	{
	private:
		int resolution;
		int binsize;
		int* bins;
		int filtersize;
		int* window;
		int* filteredBins;

	public:
		smoothing(int resolution, int binsize, int* bins, int filtersize);

		int* applyFilter();

		int median(int currentBin);

		void sorttest();

		~smoothing();
	};

}


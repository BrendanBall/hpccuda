#pragma once
#include "structs.h"
#include "cuda_runtime.h"


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
		int* filteredBins;

	public:
		smoothing(int resolution, int binsize, int* bins, int filtersize);

		int* applyFilter(float* kernelTime);

		void cudaMedianFilter(int* dev_bins, int* dev_filteredBins, float* kernelTime);

		int median(int currentBin);

		void sorttest();

		~smoothing();
	};

}


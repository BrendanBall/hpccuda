#include "binning.h"
#include <iostream>
#include <cstring>
#include <omp.h>
#include <math.h> 




hpcparallel::binning::binning(int res, char* filename) : resolution(res), inputstream(filename), numThreads(omp_get_max_threads()), chunkSize(1000000)
{
	bins = new int[res * res];
	// initialize bin array to all zeroes
	memset(bins, 0, res * res * sizeof(int));
	std::cout << "number of threads: " << numThreads << std::endl;
	
}


hpc::array<int>* hpcparallel::binning::processBin()
{

	float inverseRes = 1 / (float)resolution;
	
	int binsize = resolution * resolution;
	
	int numChunks = (int)(ceil((float)inputstream.getFileSize() / (sizeof(float) * chunkSize)));
	std::cout << "num chunks: " << numChunks << std::endl;

#pragma omp parallel 
	{
		int x;
		int y;
		float* p_floats = new float[chunkSize];
		hpc::array<float>* p_floatarr = new hpc::array<float>(chunkSize, (float*)p_floats);
		int* p_bins = new int[binsize];
		memset(p_bins, 0, binsize * sizeof(float));

#pragma omp for
		for (int chunk = 0; chunk < numChunks; ++chunk)
		{

			inputstream.nextChunk(p_floatarr);


			for (int i = 0; i < p_floatarr->size; i += 2)
			{
				// coordinates can be between 0 and 1, including 1 but there is no bin for 1;
				// integer division doesn't work for including the last upper bound so have to manually check it.
				if (p_floatarr->pointer[i] == 1)
				{
					x = resolution - 1;
				}
				else
				{
					x = (int)(p_floatarr->pointer[i] / inverseRes);

				}
				if (p_floatarr->pointer[i + 1] == 1)
				{
					y = resolution - 1;
				}
				else
				{
					y = (int)(p_floatarr->pointer[i + 1] / inverseRes);
				}
				p_bins[y * resolution + x]++;


			}
		}
#pragma omp critical
		for (int i = 0; i < binsize; ++i)
		{
			bins[i] = bins[i] + p_bins[i];
		}
	}
	


	hpc::array<int>* binarr = new hpc::array<int>(resolution*resolution, bins);
	return binarr;

}



hpcparallel::binning::~binning()
{
	delete[] bins;
}

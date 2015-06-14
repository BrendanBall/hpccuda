#include "binning.h"
#include <iostream>
#include <cstring>
#include <omp.h>




hpcparallel::binning::binning(size_t res, char* filename) : resolution(res), inputstream(filename), numThreads(omp_get_max_threads()), numFloats(1000000)
{
	bins = new int[res * res];
	// initialize bin array to all zeroes
	memset(bins, 0, res * res * sizeof(float));
	std::cout << "number of threads: " << numThreads << std::endl;
	
}


hpc::array<int>* hpcparallel::binning::processBin()
{

	float inverseRes = 1 / (float)resolution;
	unsigned int x;
	unsigned int y;
	float* floats = new float[numFloats];
	hpc::array<float>* floatarr = new hpc::array<float>(numFloats, (float*)floats);
	do 
	{
		inputstream.nextChunk(floatarr);
		

		for (unsigned int i = 0; i < floatarr->size; i += 2)
		{
			// coordinates can be between 0 and 1, including 1 but there is no bin for 1;
			// integer division doesn't work for including the last upper bound so have to manually check it.
			if (floatarr->pointer[i] == 1)
			{
				x = resolution - 1;
			}
			else
			{
				x = (int)(floatarr->pointer[i] / inverseRes);

			}
			if (floatarr->pointer[i + 1] == 1)
			{
				y = resolution - 1;
			}
			else
			{
				y = (int)(floatarr->pointer[i + 1] / inverseRes);
			}
			bins[y * resolution + x]++;


		}
		
	}
	while (floatarr->size > 0);

	//hpc::printcsv(resolution, bins);

	hpc::array<int>* binarr = new hpc::array<int>(resolution*resolution, bins);
	return binarr;

}



hpcparallel::binning::~binning()
{
	delete[] bins;
}

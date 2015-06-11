#include "binning.h"
#include <iostream>
#include <cstring>
#include "printcsv.h"




hpcserial::binning::binning(size_t res, char* filename) : resolution(res), inputstream(filename)
{
	bins = new int[res * res];
	// initialize bin array to all zeroes
	memset(bins, 0, res * res * sizeof(float));
	
}


hpc::array<int>* hpcserial::binning::processBin()
{
	hpc::array<float>* floatarr;
	float inverseRes = 1 / (float)resolution;
	unsigned int x;
	unsigned int y;
	do 
	{
		floatarr = inputstream.nextChunk();
		

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

	hpc::printcsv(resolution, bins);
	hpc::printFileCsv(resolution, bins, "points_uf.csv");

	hpc::array<int>* binarr = new hpc::array<int>(resolution*resolution, bins);
	return binarr;

}



hpcserial::binning::~binning()
{
	delete[] bins;
}

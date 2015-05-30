#include "binning.h"
#include <iostream>


hpcserial::binning::binning(size_t res, char* filename) : resolution(res), inputstream(filename)
{
	bin = new int[res * res];
	// initialize bin array to all zeroes
	memset(bin, 0, res * res * sizeof(float));
	
}


void hpcserial::binning::processBin()
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
			
			bin[y * resolution + x]++;

		}
		
	}
	while (floatarr->size > 0);

	
	unsigned int total = 0;
	for (unsigned int i = 0; i < resolution; ++i)
	{
		for (unsigned int j = 0; j < resolution; ++j)
		{
			std::cout << bin[i*resolution + j] << "\t" ;
			total += bin[i*resolution + j];

		}
		std::cout << std::endl;

	}
	std::cout << total << std::endl;

	

}



hpcserial::binning::~binning()
{
	delete[] bin;
}

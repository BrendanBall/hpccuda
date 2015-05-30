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

	do 
	{
		floatarr = inputstream.nextChunk();

		for (unsigned int i = 0; i < floatarr->size; i += 2)
		{
			unsigned int x = (int)((double)floatarr->pointer[i] / inverseRes);
			unsigned int y = (int)((double)floatarr->pointer[i + 1] / inverseRes);
			if (y >= resolution || x >= resolution)
			{
				std::cout << y << " " << floatarr->pointer[i + 1] << " " << x << " " << floatarr->pointer[i] << std::endl;

			}
			else
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

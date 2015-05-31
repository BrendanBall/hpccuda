#include "printcsv.h"
#include <iostream>

void hpc::printcsv(size_t resolution, int* bins)
{
	std::cout << ",";
	float inverseRes = 1 / (float)resolution;
	for (unsigned int i = 0; i < resolution; ++i)
	{
		if (i == resolution - 1)
		{
			std::cout << inverseRes * i;

		}
		else
		{
			std::cout << inverseRes * i << ",";
		}

	}
	std::cout << std::endl;

	for (unsigned int y = 0; y < resolution; ++y)
	{
		std::cout << inverseRes * y <<",";

		for (unsigned int x = 0; x < resolution; ++x)
		{
			if (x == resolution - 1)
			{
				std::cout << bins[y * resolution + x];

			}
			else
			{
				std::cout << bins[y * resolution + x] << ",";

			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

}

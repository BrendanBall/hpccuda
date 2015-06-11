#include "printcsv.h"
#include <iostream>
#include <stdio.h>
#include <sstream>

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

void hpc::printFileCsv(size_t resolution, int* bins, char* filename)
{
	FILE* file = fopen(filename, "w");
	std::ostringstream ss;


	if (file)
	{
		ss << ",";
		float inverseRes = 1 / (float)resolution;
		for (unsigned int i = 0; i < resolution; ++i)
		{
			if (i == resolution - 1)
			{
				ss << inverseRes * i;

			}
			else
			{
				ss << inverseRes * i << ",";
			}

		}
		ss << std::endl;

		for (unsigned int y = 0; y < resolution; ++y)
		{
			ss << inverseRes * y << ",";

			for (unsigned int x = 0; x < resolution; ++x)
			{
				if (x == resolution - 1)
				{
					ss << bins[y * resolution + x];

				}
				else
				{
					ss << bins[y * resolution + x] << ",";

				}
			}
			ss << std::endl;
		}
		ss << std::endl;
		fwrite(ss.str().c_str(), sizeof(char), strlen(ss.str().c_str()), file);
		fclose(file);
	}
	
}


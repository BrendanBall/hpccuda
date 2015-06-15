#include "printcsv.h"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <cstring>
#include <time.h>
#include <fstream>


void hpc::printcsv(int resolution, int* bins)
{
	std::cout << ",";
	float inverseRes = 1 / (float)resolution;
	for (int i = 0; i < resolution; ++i)
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

	for (int y = 0; y < resolution; ++y)
	{
		std::cout << inverseRes * y <<",";

		for (int x = 0; x < resolution; ++x)
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

void hpc::printFileCsv(int resolution, int* bins, const char* filename)
{
	std::ofstream ss(filename);


	if (ss.is_open())
	{
		ss << ",";
		float inverseRes = 1 / (float)resolution;
		for (int i = 0; i < resolution; ++i)
		{
			if (i < resolution - 1)
			{
				ss << inverseRes * i << ",";

			}
			else
			{
				ss << inverseRes * i << std::endl;

			}

		}

		for (int y = 0; y < resolution; ++y)
		{
			ss << inverseRes * y << ",";

			for (int x = 0; x < resolution; ++x)
			{
				if (x < resolution - 1)
				{
					ss << bins[y * resolution + x] << ",";


				}
				else
				{
					ss << bins[y * resolution + x];

				}
			}
			if (y != resolution - 1)
			{
				ss << std::endl;

			}
		}


	}

	
}

// from stackoverflow 
// http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
const std::string hpc::currentDateTime()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

void hpc::writeResultsFile(char* filename, int binres, int filtersize, unsigned int binningTime, unsigned int smoothingTime)
{
	std::ofstream resultsfile("results.csv", std::ios_base::app);
	if (resultsfile.is_open())
	{
		resultsfile << currentDateTime() << ",";
		resultsfile << filename << ",";
		resultsfile << binres << ",";
		resultsfile << filtersize << ",";
		resultsfile << binningTime << ",";
		resultsfile << smoothingTime << "\n";


	}
}
#include "smoothing.h"
#include <iostream>
#include <algorithm>

hpcparallel::smoothing::smoothing(size_t resolution, size_t binsize, int* bins, size_t filtersize) : resolution(resolution), binsize(binsize), bins(bins), filtersize(filtersize)
{
	window = new int[filtersize * filtersize];
	
}



void hpcparallel::smoothing::applyFilter()
{
	for (unsigned int i = 0; i < binsize; ++i)
	{
		//std::cout << i << std::endl;
		bins[i] = median(i);
	}

	//hpc::printcsv(resolution, bins);

	
}

int hpcparallel::smoothing::median(int currentBin)
{
	
	int binx = currentBin % resolution;
	int biny = currentBin / resolution;
	int halfFS = int(filtersize / 2);
	size_t startx = binx - halfFS;
	size_t starty = biny - halfFS;
	size_t endx = startx + filtersize;
	size_t endy = starty + filtersize;

	//std::cout << startx << " " << endx << " " << starty << " " <<endy << std::endl;

	

		 
	int i = 0;
	for (size_t y = starty; y < endy; ++y)
	{
		for (size_t x = startx; x < endx; ++x)
		{
			if (y >= 0 && y < resolution && x >= 0 && x < resolution)
			{
				window[i] = bins[y*resolution + x];
				++i;
			}
		}
		//std::cout << std::endl;

	}

	std::sort(window, window + i);

	/*for (int j = 0; j < filtersize * filtersize; ++j)
	{
		std::cout << window[j] << " ";
	}
	std::cout << i << std::endl << std::endl;*/
	int median;
	if (i % 2 == 0)
	{
		median = (window[i / 2] + window[(i / 2) + 1]) / 2;
	}
	else
	{
		median = window[(i / 2) + 1];
	}
	return median;
}

void hpcparallel::smoothing::sorttest()
{
	int* v = new int[10]{2,5,6,4,3,7,1,8,10,9};

	std::sort(v, v + 10);

	for (unsigned int i = 0; i < 10; ++i)
	{
		std::cout << v[i] << " ";

	}
	std::cout << std::endl;
}


hpcparallel::smoothing::~smoothing()
{
	delete[] window;
}
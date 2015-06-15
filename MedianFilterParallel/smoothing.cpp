#include "smoothing.h"
#include <iostream>
#include <algorithm>
#include <math.h> 

hpcparallel::smoothing::smoothing(int resolution, int binsize, int* bins, int filtersize) : resolution(resolution), binsize(binsize), bins(bins), filtersize(filtersize)
{
	window = new int[filtersize * filtersize];
	memset(window, 0, filtersize * filtersize * sizeof(int));
	filteredBins = new int[binsize];
	memset(filteredBins, 0, binsize * sizeof(int));


}



int* hpcparallel::smoothing::applyFilter()
{
	for (int i = 0; i < binsize; ++i)
	{
		//std::cout << i << std::endl;
		filteredBins[i] = median(i);
	}

	//hpc::printcsv(resolution, bins);

	return filteredBins;
}

int hpcparallel::smoothing::median(int currentBin)
{

	int binx = currentBin % resolution;
	int biny = currentBin / resolution;
	int halfFS = int(filtersize / 2);
	int startx = binx - halfFS;
	int starty = biny - halfFS;
	int endx = startx + filtersize;
	int endy = starty + filtersize;

	//std::cout << "binxy"<< binx << " " << biny << std::endl;

	//std::cout << startx << " " << endx << " " << starty << " " <<endy << std::endl;




	int i = 0;
	for (int y = starty; y < endy; ++y)
	{
		for (int x = startx; x < endx; ++x)
		{
			if (y >= 0 && y < resolution && x >= 0 && x < resolution)
			{
				window[i] = bins[y*resolution + x];
				++i;
			}
		}

	}


	std::sort(window, window + i);

	/*for (int j = 0; j < i; ++j)
	{
	if (j % filtersize == 0)
	{
	std::cout << std::endl;
	}
	std::cout << window[j] << " ";
	}*/


	int median;
	if (i % 2 == 0)
	{
		//throwing away decimal part of average
		median = (window[(i / 2) - 1] + window[(i / 2)]) / 2;
	}
	else
	{
		median = window[(i / 2)];
	}
	return median;
}



hpcparallel::smoothing::~smoothing()
{
	delete window;
}

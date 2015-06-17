#include "smoothing.h"
#include <iostream>
#include <algorithm>
#include <math.h> 
#include <cstring>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <thrust/sort.h>

#define BLOCKSIZE 32

hpcparallel::smoothing::smoothing(int resolution, int binsize, int* bins, int filtersize) : resolution(resolution), binsize(binsize), bins(bins), filtersize(filtersize)
{
	window = new int[filtersize * filtersize];
	memset(window, 0, filtersize * filtersize * sizeof(int));
	filteredBins = new int[binsize];
	//memset(filteredBins, 0, binsize * sizeof(int));


}

__global__ void medianFilterKernel(const int* dev_bins, int* dev_filteredBins, int resolution, int binsize, int filtersize, int halfFS, int windowsize)
{
	int tx = blockDim.x * blockIdx.x + threadIdx.x;
	int ty = blockDim.y * blockIdx.y + threadIdx.y;

	int startx = tx - halfFS;
	int starty = ty - halfFS;
	int endx = startx + filtersize;
	int endy = starty + filtersize;

	if (tx < resolution && ty < resolution)
	{
		//__shared__ int* sbins = new int[(blockDim.y + halfFS)*(blockDim.x + halfFS)];

		int* window = new int[windowsize];

		int i = 0;
		for (int y = starty; y < endy; ++y)
		{
			for (int x = startx; x < endx; ++x)
			{
				if (y >= 0 && y < resolution && x >= 0 && x < resolution)
				{
					window[i] = dev_bins[y*resolution + x];
					++i;
				}
			}
		}

		thrust::sort(thrust::seq, window, window + i);

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

		dev_filteredBins[ty * resolution + tx] = median;

	}
}

#define cudaSafe(statuscode, description) { gpuAssert(statuscode, description, __FILE__, __LINE__); }

inline void gpuAssert(cudaError_t code, char* description, char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
	  std::cout <<"Cuda error: " << description << ", " << cudaGetErrorString(code) << " " << file << " " << line << std::endl;;
	 // if (abort) exit(code);
   }
}

int* hpcparallel::smoothing::applyFilter()
{
	//for (int i = 0; i < binsize; ++i)
	//{
	//	//std::cout << i << std::endl;
	//	filteredBins[i] = median(i);
	//}
	int* dev_bins = 0;
	int* dev_filteredBins = 0;
	cudaMedianFilter(dev_bins, dev_filteredBins);
	cudaSafe(cudaDeviceReset(), "cuda device reset");
	
	return filteredBins;
}

void hpcparallel::smoothing::cudaMedianFilter(int* dev_bins, int* dev_filteredBins)
{

	cudaSafe(cudaSetDevice(0), "set device");

	cudaSafe(cudaMalloc((void**)&dev_bins, binsize * sizeof(int)), "cuda malloc dev_bins");
	cudaSafe(cudaMalloc((void**)&dev_filteredBins, binsize * sizeof(int)), "cuda malloc dev_filteredBins");

	cudaSafe(cudaMemcpy(dev_bins, bins, binsize * sizeof(int), cudaMemcpyHostToDevice), "cuda memcpy htd dev_bins");
	cudaSafe(cudaMemset(dev_filteredBins, 0, binsize * sizeof(int)), "cuda memset dev_filteredBins");
	

	dim3 numThreads(BLOCKSIZE, BLOCKSIZE);
	dim3 numBlocks;
	numBlocks.x = (int)ceil(resolution / (float)numThreads.x);
	numBlocks.y = (int)ceil(resolution / (float)numThreads.y);

	//std::cout << "numBlocks & numThreads: " << numBlocks.x << " " << numThreads.x << " " << numBlocks.x * numThreads.x << " " << resolution << std::endl;
	//std::cout << "numBlocks & numThreads: " << numBlocks.y << " " << numThreads.y << " " << numBlocks.y * numThreads.y << " " << resolution << std::endl;

	medianFilterKernel << <numBlocks, numThreads >> >(dev_bins, dev_filteredBins, resolution, binsize, filtersize, int(filtersize / 2), filtersize*filtersize);

	cudaSafe(cudaGetLastError(), "cuda launch");
	
	cudaSafe(cudaDeviceSynchronize(), "cuda device synchronize");

	cudaSafe(cudaMemcpy(filteredBins, dev_filteredBins, binsize * sizeof(int), cudaMemcpyDeviceToHost), "cuda memcpy dth dev_filteredBins");
	
	cudaSafe(cudaFree(dev_bins), "cuda free");
	cudaSafe(cudaFree(dev_filteredBins), "cuda free");
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
	delete filteredBins;
}

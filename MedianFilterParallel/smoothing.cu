#include "smoothing.h"
#include <iostream>
#include <algorithm>
#include <math.h> 
#include <cstring>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#define BLOCKSIZE 32

hpcparallel::smoothing::smoothing(int resolution, int binsize, int* bins, int filtersize) : resolution(resolution), binsize(binsize), bins(bins), filtersize(filtersize)
{
	window = new int[filtersize * filtersize];
	memset(window, 0, filtersize * filtersize * sizeof(int));
	filteredBins = new int[binsize];
	//memset(filteredBins, 0, binsize * sizeof(int));


}

__global__ void medianFilterKernel(const int* dev_bins, int* dev_filteredBins, const int resolution, const int binsize, const int filtersize, const int windowsize)
{
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;

	if (x < resolution && y < resolution)
	{
		dev_filteredBins[y*resolution + x] = 2;
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
	cudaFree(dev_bins);
	cudaFree(dev_filteredBins);
	cudaError_t cudaStatus = cudaDeviceReset();
	if (cudaStatus != cudaSuccess)
	{
		std::cout << "cudaDeviceReset failed" << std::endl;
	}
	return filteredBins;
}

cudaError_t hpcparallel::smoothing::cudaMedianFilter(int* dev_bins, int* dev_filteredBins)
{
	

	cudaError_t cudaStatus;
	cudaStatus = cudaSetDevice(0);

	if (cudaStatus != cudaSuccess)
	{
		std::cout << "Cuda failed to set cuda device as 0" << std::endl;
		return cudaStatus;
	}

	cudaStatus = cudaMalloc((void**)&dev_bins, binsize * sizeof(int));
	cudaStatus = cudaMalloc((void**)&dev_filteredBins, binsize * sizeof(int));

	if (cudaStatus != cudaSuccess)
	{
		std::cout << "cudaMalloc failed" << std::endl;
		return cudaStatus;
	}

	cudaStatus = cudaMemcpy(dev_bins, bins, binsize * sizeof(int), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
	{
		std::cout << "cudaMemcpy failed" << std::endl;
		return cudaStatus;
	}

	cudaStatus = cudaMemset(dev_filteredBins, 0, binsize * sizeof(int));
	if (cudaStatus != cudaSuccess)
	{
		std::cout << "cudaMemset failed" << std::endl;
		return cudaStatus;
	}

	dim3 numThreads(BLOCKSIZE, BLOCKSIZE);
	dim3 numBlocks;
	numBlocks.x = (int)ceil(resolution / (float)numThreads.x);
	numBlocks.y = (int)ceil(resolution / (float)numThreads.y);

	//std::cout << "numBlocks & numThreads: " << numBlocks.x << " " << numThreads.x << " " << numBlocks.x * numThreads.x << " " << resolution << std::endl;
	//std::cout << "numBlocks & numThreads: " << numBlocks.y << " " << numThreads.y << " " << numBlocks.y * numThreads.y << " " << resolution << std::endl;

	medianFilterKernel<<<numBlocks, numThreads>>>(dev_bins, dev_filteredBins, resolution, binsize, filtersize, filtersize*filtersize);

	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess)
	{
		std::cout << "kernel launch failed: \n" << cudaGetErrorString(cudaStatus) << std::endl;
		return cudaStatus;
	}

	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		std::cout << "cudaDeviceSynchronize returned error: " << cudaStatus << std::endl;
		return cudaStatus;
	}

	cudaStatus = cudaMemcpy(filteredBins, dev_filteredBins, binsize * sizeof(int), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess)
	{
		std::cout << "cudaMemcpy to host failed" << std::endl;
		return cudaStatus;
	}

	cudaFree(dev_bins);
	cudaFree(dev_filteredBins);
	return cudaStatus;
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

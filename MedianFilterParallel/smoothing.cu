#include "smoothing.h"
#include <iostream>
#include <algorithm>
#include <math.h> 
#include <cstring>
#include "cuda_runtime.h"
#include "device_functions.h"
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

__global__ void medianFilter3x3Kernel(const int* dev_bins, int* dev_filteredBins, int resolution, int binsize, int filtersize, int halfFS, int windowsize)
{
	int tx = blockDim.x * blockIdx.x + threadIdx.x;
	int ty = blockDim.y * blockIdx.y + threadIdx.y;
	int tlx = threadIdx.x;
	int tly = threadIdx.y;

	if (tx < resolution && ty < resolution)
	{
		//shared array is 1 row/column bigger on every side for edge cases.
		__shared__ int sm_bins[BLOCKSIZE + 2][BLOCKSIZE + 2];

		//populate shared memory block
		sm_bins[threadIdx.y + 1][threadIdx.x + 1] = dev_bins[ty*resolution + tx];

		//get edge values needed for sliding window
		bool tx_left_edge = (tlx == 0);
		bool ty_top_edge = (tly == 0);
		bool tx_right_edge = (tlx == BLOCKSIZE - 1);
		bool ty_bot_edge = (tly == BLOCKSIZE - 1);

		// -1 padding for values not in dev_bins
		if (tx_left_edge)
			sm_bins[tly + 1][tlx] = -1;
		else if (tx_right_edge)
			sm_bins[tly + 1][tlx + 2] = -1;
		if (ty_top_edge)
		{
			sm_bins[tly][tlx + 1] = -1;
			if (tx_left_edge)
				sm_bins[tly][tlx] = -1;
			else if (tx_right_edge)
				sm_bins[tly][tlx + 2] = -1;

		}
		else if (ty_bot_edge)
		{
			sm_bins[tly + 2][tlx + 1] = -1;
			if (tx_left_edge)
				sm_bins[tly + 2][tlx] = -1;
			else if (tx_right_edge)
				sm_bins[tly + 2][tlx + 2] = -1;
		}


		//check if shared memory edge is global edge and then don't include it
		tx_left_edge &= (tx > 0);
		tx_right_edge &= (tx < resolution - 1);
		ty_top_edge &= (ty > 0);
		ty_bot_edge &= (ty < resolution - 1);

		// pull values into shared memory
		if (tx_left_edge)
			sm_bins[tly + 1][tlx] = dev_bins[ty*resolution + tx - 1];
		else if (tx_right_edge)
			sm_bins[tly + 1][tlx + 2] = dev_bins[ty*resolution + tx + 1];
		if (ty_top_edge)
		{
			sm_bins[tly][tlx + 1] = dev_bins[(ty - 1)*resolution + tx];
			if (tx_left_edge)
				sm_bins[tly][tlx] = dev_bins[(ty - 1)*resolution + tx - 1];
			else if (tx_right_edge)
				sm_bins[tly][tlx + 2] = dev_bins[(ty - 1)*resolution + tx + 1];

		}
		else if (ty_bot_edge)
		{
			sm_bins[tly + 2][tlx + 1] = dev_bins[(ty + 1)*resolution + tx];
			if (tx_left_edge)
				sm_bins[tly + 2][tlx] = dev_bins[(ty + 1)*resolution + tx - 1];
			else if (tx_right_edge)
				sm_bins[tly + 2][tlx + 2] = dev_bins[(ty + 1)*resolution + tx + 1];
		}

		__syncthreads();

		int window[9] =
		{
			sm_bins[threadIdx.y][threadIdx.x], sm_bins[threadIdx.y][threadIdx.x + 1], sm_bins[threadIdx.y][threadIdx.x + 2],
			sm_bins[threadIdx.y + 1][threadIdx.x], sm_bins[threadIdx.y + 1][threadIdx.x + 1], sm_bins[threadIdx.y + 1][threadIdx.x + 2],
			sm_bins[threadIdx.y + 2][threadIdx.x], sm_bins[threadIdx.y + 2][threadIdx.x + 1], sm_bins[threadIdx.y + 2][threadIdx.x + 2]
		};

	
		//thrust::sort(thrust::seq, window, window + 9);

		//bubble sort
		for (int i = 0; i < 9; ++i){
		
			for (int j = i + 1; j < 9; ++j){
				if (window[i] > window[j])
				{
					int temp = window[i];
					window[i] = window[j];
					window[j] = temp;
				}
			}
		}
		
		int edges = 0; // count number of elements equal to -1 which are not in the global array
		for (int i = 0; i < 9; ++i)
		{
			if (window[i] == -1)
				edges++;
		}

		int median;
		int i = (9 - edges);
		if (i % 2 == 0)
		{
			//throwing away decimal part of average
			median = ((window[(i / 2) - 1 + edges] + window[(i / 2) + edges]) / 2);
		}
		else
		{
			median = window[(i / 2) + edges];
		}
		
			
		dev_filteredBins[ty * resolution + tx] = median;
	}
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
	struct cudaFuncAttributes funcAttrib;
	cudaSafe(cudaFuncGetAttributes(&funcAttrib, medianFilter3x3Kernel), "cudafuncgetattributes");
	printf("%s numRegs=%d\n", "medianFilter3x3Kernel", funcAttrib.numRegs);
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

	//medianFilterKernel << <numBlocks, numThreads >> >(dev_bins, dev_filteredBins, resolution, binsize, filtersize, int(filtersize / 2), filtersize*filtersize);
	medianFilter3x3Kernel << <numBlocks, numThreads >> >(dev_bins, dev_filteredBins, resolution, binsize, 3, 1, 9);

	cudaSafe(cudaGetLastError(), "cuda launch");
	
	cudaSafe(cudaDeviceSynchronize(), "cuda device synchronize");

	cudaSafe(cudaMemcpy(filteredBins, dev_filteredBins, binsize * sizeof(int), cudaMemcpyDeviceToHost), "cuda memcpy dth dev_filteredBins");
	
	cudaSafe(cudaFree(dev_bins), "cuda free");
	cudaSafe(cudaFree(dev_filteredBins), "cuda free");
}


hpcparallel::smoothing::~smoothing()
{
	delete window;
	delete filteredBins;
}

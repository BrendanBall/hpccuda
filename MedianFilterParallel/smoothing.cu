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
		//edge values for shared memory are values needed by the window but whose median is not calculated in this block.
		//shared array is 1 row/column bigger on every side for edge cases.
		__shared__ int sm_bins[BLOCKSIZE + 2][BLOCKSIZE + 2];

		//populate shared memory block values which are not edge values
		sm_bins[tly + 1][tlx + 1] = dev_bins[ty*resolution + tx];

		//check if index on the edge of the block
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

		// pull edge values into shared memory using threads on edges of block
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
			sm_bins[tly][tlx], sm_bins[tly][tlx + 1], sm_bins[tly][tlx + 2],
			sm_bins[tly + 1][tlx], sm_bins[tly + 1][tlx + 1], sm_bins[tly + 1][tlx + 2],
			sm_bins[tly + 2][tlx], sm_bins[tly + 2][tlx + 1], sm_bins[tly + 2][tlx + 2]
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
		
		int edges = 0; // count number of elements equal to -1, values which are not in the global array
		for (int i = 0; i < 9; ++i)
		{
			if (window[i] == -1)
				edges++;
		}

		// the median is calculated only with values actually inside the array, values outside array are -1
		// for even number of values in window calculate average between the 2 middle values
		int median;
		int mi = (9 - edges);
		if (mi % 2 == 0)
		{
			//throwing away decimal part of average
			median = ((window[(mi / 2) - 1 + edges] + window[(mi / 2) + edges]) / 2);
		}
		else
		{
			median = window[(mi / 2) + edges];
		}
		
			
		dev_filteredBins[ty * resolution + tx] = median;
	}
}

template<int WINDOWSIZE, int FILTERSIZE, int EDGESIZE>
__global__ void medianFilterTemplateKernel(const int* dev_bins, int* dev_filteredBins, int resolution, int binsize)
{
	int tx = blockDim.x * blockIdx.x + threadIdx.x;
	int ty = blockDim.y * blockIdx.y + threadIdx.y;
	int tlx = threadIdx.x;
	int tly = threadIdx.y;

	// edge size is Filtersize/2 integer division, half the length of the window excluding the value we calculating in the middle
	
	if (tx < resolution && ty < resolution)
	{
		__shared__ int sm_bins[BLOCKSIZE + FILTERSIZE - 1][BLOCKSIZE + FILTERSIZE - 1];

		//populate shared memory
		//first populate values which fall inside the block (not edge values)
		sm_bins[tly + EDGESIZE][tlx + EDGESIZE] = dev_bins[ty*resolution + tx];

		//check if index on the edge of the block
		// in this case the edge of the block is the border of the block with thickness equal to edgesize
		bool tx_left_edge = (tlx < EDGESIZE);
		bool ty_top_edge = (tly < EDGESIZE);
		bool tx_right_edge = (tlx > BLOCKSIZE - EDGESIZE - 1);
		bool ty_bot_edge = (tly > BLOCKSIZE - EDGESIZE - 1);

		// -1 padding for edge values
		if (tx_left_edge)
			sm_bins[tly + EDGESIZE][tlx] = -1;
		else if (tx_right_edge)
			sm_bins[tly + EDGESIZE][tlx + (2 * EDGESIZE)] = -1;
		if (ty_top_edge)
		{
			sm_bins[tly][tlx + EDGESIZE] = -1;
			if (tx_left_edge)
				sm_bins[tly][tlx] = -1;
			else if (tx_right_edge)
				sm_bins[tly][tlx + (2 * EDGESIZE)] = -1;

		}
		else if (ty_bot_edge)
		{
			sm_bins[tly + (2 * EDGESIZE)][tlx + EDGESIZE] = -1;
			if (tx_left_edge)
				sm_bins[tly + (2 * EDGESIZE)][tlx] = -1;
			else if (tx_right_edge)
				sm_bins[tly + (2 * EDGESIZE)][tlx + (2 * EDGESIZE)] = -1;
		}

		//check if shared memory edge is global edge and then don't include it
		tx_left_edge &= (tx - EDGESIZE  > 0);
		tx_right_edge &= (tx + EDGESIZE < resolution);
		ty_top_edge &= (ty - EDGESIZE > 0);
		ty_bot_edge &= (ty + EDGESIZE < resolution);

		//populate edge values
		if (tx_left_edge)
			sm_bins[tly + EDGESIZE][tlx] = dev_bins[ty*resolution + tx - EDGESIZE];
		else if (tx_right_edge)
			sm_bins[tly + EDGESIZE][tlx + (2 * EDGESIZE)] = dev_bins[ty*resolution + tx + EDGESIZE];
		if (ty_top_edge)
		{
			sm_bins[tly][tlx + EDGESIZE] = dev_bins[(ty - EDGESIZE)*resolution + tx];
			if (tx_left_edge)
				sm_bins[tly][tlx] = dev_bins[(ty - EDGESIZE)*resolution + tx - EDGESIZE];
			else if (tx_right_edge)
				sm_bins[tly][tlx + (2 * EDGESIZE)] = dev_bins[(ty - EDGESIZE)*resolution + tx + EDGESIZE];

		}
		else if (ty_bot_edge)
		{
			sm_bins[tly + (2 * EDGESIZE)][tlx + EDGESIZE] = dev_bins[(ty + EDGESIZE)*resolution + tx];
			if (tx_left_edge)
				sm_bins[tly + (2 * EDGESIZE)][tlx] = dev_bins[(ty + EDGESIZE)*resolution + tx - EDGESIZE];
			else if (tx_right_edge)
				sm_bins[tly + (2 * EDGESIZE)][tlx + (2 * EDGESIZE)] = dev_bins[(ty + EDGESIZE)*resolution + tx + EDGESIZE];
		}

		__syncthreads();

		

		// create window for this thread index
		int window[WINDOWSIZE];

		int startx = tlx;
		int starty = tly;
		int endx = tlx + FILTERSIZE;
		int endy = tly + FILTERSIZE;

		int i = 0;
		for (int y = starty; y < endy; ++y)
		{
			for (int x = startx; x < endx; ++x)
			{
				window[i] = sm_bins[y][x];
				i++;
				
			}
		}

		/*if (tx == 0 && ty == 0)
		{
			printf("0x0y\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n\n", window[0], window[1], window[2], window[3], window[4], window[5], window[6], window[7], window[8], window[9], window[10], window[11], window[12], window[13], window[14], window[15], window[16], window[17], window[18], window[19], window[20], window[21], window[22], window[23], window[24]);
		}
		if (tx == 31 && ty == 0)
		{
			printf("31x0y\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n\n", window[0], window[1], window[2], window[3], window[4], window[5], window[6], window[7], window[8], window[9], window[10], window[11], window[12], window[13], window[14], window[15], window[16], window[17], window[18], window[19], window[20], window[21], window[22], window[23], window[24]);
		}
		if (tx == 0 && ty == 31)
		{
			printf("0x31y\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n\n", window[0], window[1], window[2], window[3], window[4], window[5], window[6], window[7], window[8], window[9], window[10], window[11], window[12], window[13], window[14], window[15], window[16], window[17], window[18], window[19], window[20], window[21], window[22], window[23], window[24]);
		}
		if (tx == 31 && ty == 31)
		{
			printf("31x31y\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n\n", window[0], window[1], window[2], window[3], window[4], window[5], window[6], window[7], window[8], window[9], window[10], window[11], window[12], window[13], window[14], window[15], window[16], window[17], window[18], window[19], window[20], window[21], window[22], window[23], window[24]);
		}*/

		//sort window
		thrust::sort(thrust::seq, window, window + WINDOWSIZE);

		int edges = 0; // count number of elements equal to -1, values which are not in the global array

		for (int i = 0; i < WINDOWSIZE; ++i)
		{
			if (window[i] == -1)
				edges++;
		}
		// the median is calculated only with values actually inside the array, values outside array are -1
		// for even number of values in window calculate average between the 2 middle values
		int median;
		int mi = (WINDOWSIZE - edges);

		if (mi % 2 == 0)
		{
			//throwing away decimal part of average
			median = ((window[(mi / 2) - 1 + edges] + window[(mi / 2) + edges]) / 2);
		}
		else
		{
			median = window[(mi / 2) + edges];
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
	//struct cudaFuncAttributes funcAttrib;
	//cudaSafe(cudaFuncGetAttributes(&funcAttrib, medianFilterTemplateKernel<20>), "cudafuncgetattributes");
	//printf("%s numRegs=%d\n", "medianFilterTemplateKernel", funcAttrib.numRegs);
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
	
	switch (filtersize)
	{
		case 3:
			medianFilter3x3Kernel << <numBlocks, numThreads >> >(dev_bins, dev_filteredBins, resolution, binsize, 3, 1, 9);
			break;
		case 5:
			medianFilterTemplateKernel<25, 5, 2> << <numBlocks, numThreads >> >(dev_bins, dev_filteredBins, resolution, binsize);
			break;
		case 7:
			medianFilterTemplateKernel<49, 7, 3> << <numBlocks, numThreads >> >(dev_bins, dev_filteredBins, resolution, binsize);
			break;
		case 15:
			medianFilterTemplateKernel<225, 15, 7> << <numBlocks, numThreads >> >(dev_bins, dev_filteredBins, resolution, binsize);
			break;
		case 17:
			medianFilterTemplateKernel<289, 17, 8> << <numBlocks, numThreads >> >(dev_bins, dev_filteredBins, resolution, binsize);
			break;
		case 19:
			medianFilterTemplateKernel<361, 19, 9> << <numBlocks, numThreads >> >(dev_bins, dev_filteredBins, resolution, binsize);
			break;
		case 21:
			medianFilterTemplateKernel<441, 21, 10> << <numBlocks, numThreads >> >(dev_bins, dev_filteredBins, resolution, binsize);
			break;
	}
	

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

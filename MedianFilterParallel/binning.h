#pragma once
#include "inputstream.h"
#include "structs.h"

namespace hpcparallel
{
	class binning
	{

	private:
		int* bins;
		size_t resolution;
		hpcparallel::inputstream inputstream;
		int numThreads;
		int chunkSize;// buffer size


	public:
		binning(size_t res, char* filename);

		hpc::array<int>* processBin();

		~binning();
	};
}


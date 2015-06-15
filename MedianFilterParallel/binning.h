#pragma once
#include "inputstream.h"
#include "structs.h"

namespace hpcparallel
{
	class binning
	{

	private:
		int* bins;
		int resolution;
		hpcparallel::inputstream inputstream;
		int numThreads;
		int chunkSize;// buffer size


	public:
		binning(int res, char* filename);

		hpc::array<int>* processBin();

		~binning();
	};
}


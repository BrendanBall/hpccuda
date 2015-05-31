#pragma once
#include "inputstream.h"
#include "structs.h"

namespace hpcserial
{
	class binning
	{

	private:
		int* bins;
		size_t resolution;
		hpcserial::inputstream inputstream;

	public:
		binning(size_t res, char* filename);

		hpc::array<int>* processBin();

		~binning();
	};
}


#pragma once
#include "inputstream.h"
#include "structs.h"

namespace hpcserial
{
	class binning
	{

	private:
		int* bins;
		int resolution;
		hpcserial::inputstream inputstream;

	public:
		binning(int res, char* filename);

		hpc::array<int>* processBin();

		~binning();
	};
}


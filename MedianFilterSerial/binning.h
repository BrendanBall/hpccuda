#pragma once
#include "inputstream.h"
#include "structs.h"

namespace hpcserial
{
	class binning
	{

	private:
		int* bin;
		size_t resolution;
		hpcserial::inputstream inputstream;

	public:
		binning(size_t res, char* filename);

		void processBin();

		~binning();
	};
}


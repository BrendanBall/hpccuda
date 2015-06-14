#pragma once
#include <iostream>
#include <fstream>
#include "structs.h"
namespace hpcparallel{
	class inputstream
	{
	private:
		char* filename;
		int numFloats;
		float* floats;
		hpc::array<float>* floatarr;
		FILE* file;
	public:
		inputstream(char* filename);

		hpc::array<float>* nextChunk();

		~inputstream();
	};
}


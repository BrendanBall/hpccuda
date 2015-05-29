#include <iostream>
#include <fstream>
#include "structs.h"
#pragma once
namespace hpcserial{
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


#include <iostream>
#include <fstream>
#include "structs.h"
#pragma once
namespace hpcserial{
	class inputstream
	{
	private:
		char* filename;
		std::ifstream binfile;
		char* memblock;
		int numFloats;
	public:
		inputstream(char* filename);

		hpc::array<float>* nextChunk();

		~inputstream();
	};
}


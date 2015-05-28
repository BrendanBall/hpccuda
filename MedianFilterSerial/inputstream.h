#include <iostream>
#include <fstream>
#include "structs.h"
#pragma once
namespace hpcserial{
	class inputstream
	{
	private:
		std::string filename;
		std::ifstream binfile;
		char* memblock;
		int numFloats;
	public:
		inputstream(const std::string &filename);

		hpc::array<float>* nextChunk();

		~inputstream();
	};
}


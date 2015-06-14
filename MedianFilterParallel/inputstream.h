#pragma once
#include <iostream>
#include <fstream>
#include "structs.h"
#include <mutex>

namespace hpcparallel{
	class inputstream
	{
	private:
		std::mutex mtx;
		char* filename;
		int numFloats;
		size_t filesize;
		//float* floats;
		//hpc::array<float>* floatarr;
		FILE* file;
	public:
		inputstream(char* filename);

		void nextChunk(hpc::array<float>* floatarr);

		size_t getFileSize();
		~inputstream();
	};
}


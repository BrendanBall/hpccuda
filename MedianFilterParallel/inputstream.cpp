#include "inputstream.h"
#include <sys/stat.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#define stat64 _stat64
#endif


hpcparallel::inputstream::inputstream(char* filename) : mtx(), filename(filename)
{
	file = fopen(filename, "rb");
	if (!file)
	{
		std::cout << "file not found, filename: "<< filename << std::endl;
		exit(1);
	}

	numFloats = 1000000; // buffer size
	//floats = new float[numFloats];
	//floatarr = new hpc::array<float>(numFloats, (float*)floats);

	struct stat64 st;
	stat64(filename, &st);
	filesize = st.st_size;
	//std::cout << "filesize: " << filesize << std::endl;


}

size_t hpcparallel::inputstream::getFileSize()
{
	return filesize;
}

void hpcparallel::inputstream::nextChunk(hpc::array<float>* floatarr)
{
	
	mtx.lock();
	if (file)
	{

		int remainingfloats = fread((void*)floatarr->pointer, sizeof(float), numFloats, file);
		floatarr->size = remainingfloats;


	}
	else
	{
		floatarr = nullptr;
	}
	mtx.unlock();

}



hpcparallel::inputstream::~inputstream()
{
	//delete[] floats;
	//delete floatarr;
}

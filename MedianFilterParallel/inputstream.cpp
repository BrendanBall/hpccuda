#include "inputstream.h"
#include <sys/stat.h>


hpcparallel::inputstream::inputstream(char* filename) : mtx(), filename(filename)
{
	file = fopen(filename, "rb");
	numFloats = 1000000; // buffer size
	//floats = new float[numFloats];
	//floatarr = new hpc::array<float>(numFloats, (float*)floats);

	struct stat st;
	stat(filename, &st);
	filesize = st.st_size;
	std::cout << "filesize: " << filesize << std::endl;


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
	fclose(file);
}

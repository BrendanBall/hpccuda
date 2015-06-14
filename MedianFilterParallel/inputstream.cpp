#include "inputstream.h"


hpcparallel::inputstream::inputstream(char* filename) : filename(filename)
{
	file = fopen(filename, "rb");
	numFloats = 1000000; // buffer size
	floats = new float[numFloats];
	floatarr = new hpc::array<float>(numFloats, (float*)floats);

}

hpc::array<float>* hpcparallel::inputstream::nextChunk()
{
	
	if (file)
	{
		int remainingfloats = fread((void*)floats, sizeof(float), numFloats, file);
		floatarr->size = remainingfloats;
		return floatarr;
	}
	else
	{
		return nullptr;
	}
}



hpcparallel::inputstream::~inputstream()
{
	delete[] floats;
	delete floatarr;
	fclose(file);
}

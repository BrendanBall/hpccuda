#include "inputstream.h"


hpcserial::inputstream::inputstream(char* filename) : filename(filename)
{
	file = fopen(filename, "rb");
	numFloats = 1000000;
	floats = new float[numFloats];
	floatarr = new hpc::array<float>(numFloats, (float*)floats);

}

hpc::array<float>* hpcserial::inputstream::nextChunk()
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



hpcserial::inputstream::~inputstream()
{
	delete[] floats;
	delete floatarr;
	fclose(file);
}

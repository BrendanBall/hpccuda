#include "inputstream.h"


hpcserial::inputstream::inputstream(char* filename) : filename(filename)
{
	file = fopen(filename, "rb");
	numFloats = 2e1;
	floats = new float[numFloats];
	
}

hpc::array<float>* hpcserial::inputstream::nextChunk()
{
	
	if (file)
	{
		floatarr = new hpc::array<float>(numFloats, (float*)floats);
		fread((void*)floats, sizeof(float), numFloats, file);
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

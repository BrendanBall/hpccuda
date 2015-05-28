#include "inputstream.h"


hpcserial::inputstream::inputstream(const std::string &filename) : filename(filename)
{
	binfile.open(filename, std::ios::in | std::ios::binary);
	numFloats = 10;
	memblock = new char[numFloats*sizeof(float)];
	
}

hpc::array<float>* hpcserial::inputstream::nextChunk()
{
	if (binfile.is_open())
	{
		hpc::array<float>* farr = new hpc::array<float>(numFloats, (float*)memblock);

		binfile.read(memblock, numFloats*sizeof(float));

		return farr;

	}
	else
	{
		return nullptr;
	}
}



hpcserial::inputstream::~inputstream()
{
	binfile.close();
}

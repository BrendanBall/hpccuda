#include "inputstream.h"
#include "structs.h"


hpcserial::inputstream::inputstream(const std::string &filename) : filename(filename)
{
	binfile.open(filename, std::ios::in | std::ios::binary);
	if (binfile.is_open())
	{
		int numFloats = 10;
		char* memblock = new char[numFloats*sizeof(float)];

		hpc::array<float> farr(numFloats, (float*)memblock);

		binfile.read(memblock, numFloats*sizeof(float));

		for (int i = 0; i < numFloats; ++i)
		{
			std::cout << farr.pointer[i] << std::endl;

		}
		delete[] memblock;
	}
}


hpcserial::inputstream::~inputstream()
{
	binfile.close();
}

#include "inputstream.h"


hpcserial::inputstream::inputstream(const std::string &filename) : filename(filename)
{
	binfile.open(filename, std::ios::in | std::ios::binary);
	if (binfile.is_open())
	{
		int numFloats = 10;
		char* memblock = new char[numFloats*sizeof(float)];
		float* f = (float*)memblock;
		binfile.read(memblock, numFloats*sizeof(float));

		for (int i = 0; i < numFloats; ++i)
		{
			std::cout << f[i] << std::endl;

		}
		delete[] memblock;
	}
}


hpcserial::inputstream::~inputstream()
{
	binfile.close();
}

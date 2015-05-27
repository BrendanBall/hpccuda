#include "inputstream.h"
#include <stdint.h>
#include <bitset>


hpcserial::inputstream::inputstream(const std::string &filename) : filename(filename)
{
	binfile.open(filename, std::ios::in | std::ios::binary);
	if (binfile.is_open())
	{
		int numFloats = 4;
		char* memblock = new char[numFloats*sizeof(float)];
		float* f = new float[numFloats];
		binfile.read(memblock, numFloats*sizeof(float));
		memcpy(&f, &memblock, sizeof(f));

		for (int i = 0; i < numFloats; ++i)
		{
			std::cout << f[i] << std::endl;

		}

		delete[] memblock;
		delete[] f;
	}
}


hpcserial::inputstream::~inputstream()
{
	binfile.close();
}

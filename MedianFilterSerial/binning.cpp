#include "binning.h"
#include <iostream>


hpcserial::binning::binning(size_t res, char* filename) : resolution(res), inputstream(filename)
{
	bin = new int[res * res];
	
}


void hpcserial::binning::processBin()
{
	int numbuffers = 0;
	hpc::array<float>* floatarr = inputstream.nextChunk();
	while (floatarr->size > 0)
	{
		numbuffers++;
		floatarr = inputstream.nextChunk();
	}

}



hpcserial::binning::~binning()
{
	delete[] bin;
}

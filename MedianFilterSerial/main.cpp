#include <iostream>
#include <sstream>
#include "binning.h"
#include "smoothing.h"
#include "timer.h"

bool isLittleEndian()
{
	int n = 1;
	// little endian if true
	if (*(char *)&n == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int main(int argc, char* argv[])
{
	std::cout << "hello" << std::endl;
	// This program will only work on systems using little endian as the binary data will be copied straight into memory as floats
	if (isLittleEndian())
	{
		if (argc >= 4)
		{
			char* filename = argv[1];
			size_t binres;
			size_t filtersize;
			std::stringstream str_res(argv[2]);
			std::stringstream str_fsize(argv[3]);
			if (str_res >> binres && str_fsize >> filtersize)
			{
				hpcserial::binning binning(binres, filename);
				timer timer;

				hpc::array<int>* binarr = binning.processBin();

				unsigned int time = timer.Stop();
				std::cout << "time: " << time << std::endl;

				hpcserial::smoothing smoothing(binres, binarr->size, binarr->pointer, filtersize);

				timer.Start();

				smoothing.applyFilter();

				time = timer.Stop();
				std::cout << "time: " << time << std::endl;

				std::cout << "done" << std::endl;

			}

		}
		else
		{
			std::cout << "Pass filename as first commandline arg";
		}
		
	}
	else
	{
		std::cout << "System uses big endian, this program only supports little endian" << std::endl;
	}
	std::cin.get();
	return 0;
}


#include <iostream>
#include <sstream>
#include "binning.h"

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
		if (argc >= 3)
		{
			char* filename = argv[1];
			int binres;
			std::stringstream int_str(argv[2]);
			if (int_str >> binres)
			{
				hpcserial::binning binning(binres, filename);
				binning.processBin();
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


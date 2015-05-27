#include <iostream>
#include "inputstream.h"

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
		std::string filename = "H:\\documents\\csc4000w\\HPC\\Points_[1.0e+08]_Noise_[030]_Normal.bin";
		hpcserial::inputstream inputstream(filename);
	}
	else
	{
		std::cout << "System uses big endian, this program only supports little endian" << std::endl;
	}
	std::cin.get();
	return 0;
}


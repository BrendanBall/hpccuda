#include <iostream>
#include "inputstream.h"
#include "structs.h"

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
		if (argc >= 2)
		{
			char* filename = argv[1];
			hpcserial::inputstream inputstream(filename);

			hpc::array<float>* chunk = inputstream.nextChunk();


			for (unsigned int i = 0; i < chunk -> size; ++i)
			{
				std::cout << chunk -> pointer[i] << std::endl;

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


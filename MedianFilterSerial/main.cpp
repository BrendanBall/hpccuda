#include <iostream>
#include "inputstream.h"

int main(int argc, char* argv[])
{
	std::cout << "hello" << std::endl;
	hpcserial::inputstream inputstream("something.bin");
	return 0;
}
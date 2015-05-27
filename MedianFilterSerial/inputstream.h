#include <iostream>
#include <fstream>
#pragma once
namespace hpcserial{
	class inputstream
	{
	private:
		std::string filename;
		std::ifstream binfile;
	public:
		inputstream(const std::string &filename);


		~inputstream();
	};
}


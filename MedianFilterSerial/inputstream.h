#include <iostream>
#pragma once
namespace hpcserial{
	class inputstream
	{
	private:
		std::string filename;
	public:
		inputstream(std::string filename);


		~inputstream();
	};
}


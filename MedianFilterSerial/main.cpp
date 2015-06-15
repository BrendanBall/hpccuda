#include <iostream>
#include <sstream>
#include "binning.h"
#include "smoothing.h"
#include "timer.h"
#include "printcsv.h"
#include <string>
#include <sstream>




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



std::string getBaseName(char* filepath)
{
	std::string path(filepath);
	int start = path.find_last_of("\\/") == size_t(-1) ? 0 : path.find_last_of("\\/") + 1;
	std::string name = path.substr(start, path.find_last_of("."));
	return name;
}


int main(int argc, char* argv[])
{
	std::cout << "HPC Assignment - Binning and median filter" << std::endl;
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
				getBaseName(filename);
				std::cout << "binning starting" << std::endl;

				timer timer;
				hpcserial::binning binning(binres, filename);
				hpc::array<int>* binarr = binning.processBin();
				unsigned int binningTime = timer.Stop();

				std::cout << "binning complete" << std::endl;
				std::cout << "time for binning: " << binningTime << std::endl;
				std::cout << "writing bins to csv" << std::endl;


				std::ostringstream ufss;
				ufss << getBaseName(filename) << "_" << binres << "_" << filtersize << "_uf.csv";
				hpc::printFileCsv(binres, binarr->pointer, ufss.str().c_str());
				//hpc::printcsv(binres, binarr->pointer);
				std::cout << "smoothing starting" << std::endl;

				timer.Start();
				hpcserial::smoothing smoothing(binres, binarr->size, binarr->pointer, filtersize);
				smoothing.applyFilter();
				unsigned int smoothingTime = timer.Stop();

				std::cout << "smoothing complete" << std::endl;
				std::cout << "time for smoothing: " << smoothingTime << std::endl;


				hpc::writeResultsFile(filename, binres, filtersize, binningTime, smoothingTime);

				std::cout << "writing bins to csv" << std::endl;
				std::ostringstream fss;

				fss << getBaseName(filename) << "_" << binres << "_" << filtersize << "_f.csv";

				hpc::printFileCsv(binres, binarr->pointer, fss.str().c_str());

				std::cout << "done" << std::endl;

			}

		}
		else
		{
			std::cout << "commandline arguments: <filename> <resolution> <filter_size>";
		}
		
	}
	else
	{
		std::cout << "System uses big endian, this program only supports little endian" << std::endl;
	}
	std::cin.get();
	return 0;
}


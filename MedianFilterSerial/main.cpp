#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include "binning.h"
#include "smoothing.h"
#include "timer.h"
#include "printcsv.h"


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

// from stackoverflow 
// http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
const std::string currentDateTime()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

void writeResultsFile(char* filename, size_t binres, size_t filtersize, unsigned int binningTime, unsigned int smoothingTime)
{
	std::ofstream resultsfile("results.csv", std::ios_base::app);
	if (resultsfile.is_open())
	{
		resultsfile << currentDateTime() << ",";
		resultsfile << filename << ",";
		resultsfile << binres << ",";
		resultsfile << filtersize << ",";
		resultsfile << binningTime << ",";
		resultsfile << smoothingTime << "\n";

		
	}
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
				std::cout << filename << std::endl;

				std::cout << "binning starting" << std::endl;

				timer timer;
				hpcserial::binning binning(binres, filename);
				hpc::array<int>* binarr = binning.processBin();

				unsigned int binningTime = timer.Stop();
				std::cout << "binning complete" << std::endl;
				std::cout << "time for binning: " << binningTime << std::endl;
				std::cout << "writing bins to csv" << std::endl;

				hpc::printFileCsv(binres, binarr->pointer, "points_uf.csv");

				std::cout << "smoothing starting" << std::endl;

				timer.Start();
				hpcserial::smoothing smoothing(binres, binarr->size, binarr->pointer, filtersize);
				smoothing.applyFilter();

				unsigned int smoothingTime = timer.Stop();
				std::cout << "smoothing complete" << std::endl;
				std::cout << "time for smoothing: " << smoothingTime << std::endl;


				writeResultsFile(filename, binres, filtersize, binningTime, smoothingTime);

				std::cout << "writing bins to csv" << std::endl;
				hpc::printFileCsv(binres, binarr->pointer, "points_f.csv");

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


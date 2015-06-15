#pragma once
#include <cstddef>
#include <string>

namespace hpc
{
	void printcsv(int resolution, int* bins);

	void printFileCsv(int resolution, int* bins, const char* filename);

	const std::string currentDateTime();

	void writeResultsFile(char* filename, int binres, int filtersize, unsigned int binningTime, unsigned int smoothingTime);
}


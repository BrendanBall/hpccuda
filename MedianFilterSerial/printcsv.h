#pragma once
#include <cstddef>
#include <string>

namespace hpc
{
	void printcsv(size_t resolution, int* bins);

	void printFileCsv(size_t resolution, int* bins, const char* filename);

	const std::string currentDateTime();

	void writeResultsFile(char* filename, size_t binres, size_t filtersize, unsigned int binningTime, unsigned int smoothingTime);
}


// Copied and modified this timer code from HPC workshop code
#include "timer.h"

#ifdef _WIN32
#include <Windows.h>

timer::timer()
{
	startTime = GetTickCount();
}

void timer::Start() { startTime = GetTickCount(); }
unsigned int timer::Stop() { return GetTickCount() - startTime; }

timer::~timer()
{
}

#elif __linux__

timer::timer() 
{ 
	Start(); 
}

void timer::Start()
{
	gettimeofday(&start, NULL); 
	clock_gettime(CLOCK_MONOTONIC, &pstart);
}

unsigned int timer::Stop()
{
	gettimeofday(&end, NULL); 
	return ((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000.0) + 0.5;
}
timer::~timer() {}

#endif

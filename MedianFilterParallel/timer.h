#pragma once

#ifdef __linux__
	#include <sys/time.h>
	#include <time.h>
#endif

class timer
{
private:
	unsigned int startTime;
	#ifdef __linux__
		struct timeval start, end, timeval;
		struct timespec pstart, pend;
	#endif
public:
	timer();
	void Start();
	unsigned int Stop();
	~timer();
};




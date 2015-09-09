#include <ctime>

#ifndef __TIMING_H__
#define __TIMING_H__

/*
	//Usage:
	Timer time1;
	time1.Start();
	// Timing code 1
	printf("This is a test.\n");
	time1.Stop();
	time1.Start();
	// Timing code 2
	printf("This is a test.\n");
	time1.Stop();
	printf("Total time:%f s\n", time1.TotalInSeconds());
	printf("Total time:%f ms\n", time1.TotalInMilliseconds());
	printf("Last time:%f s\n", time1.LastInSeconds());
	printf("Last time:%f ms\n", time1.LastInMilliseconds());
*/

struct Timer
{
	clock_t before;
	clock_t total;
	clock_t last;

	Timer() : before(0), total(0) {}
	void Start()
	{
		before = clock();
	}

	void Stop()
	{
		total += (last = clock() - before);
	}

	double TotalInSeconds()
	{
		return double(total) / CLOCKS_PER_SEC;
	}

	double TotalInMilliseconds()
	{
		double CLOCKS_PER_MSEC = CLOCKS_PER_SEC / 1000.0;
		return double(total) / CLOCKS_PER_MSEC;
	}

	double LastInSeconds()
	{
		return double(last) / CLOCKS_PER_SEC;
	}

	double LastInMilliseconds()
	{
		double CLOCKS_PER_MSEC = CLOCKS_PER_SEC / 1000.0;
		return double(last) / CLOCKS_PER_MSEC;
	}
};




#endif
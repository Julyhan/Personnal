#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main()
{
	struct timeval t1;
	struct timeval t2;
	gettimeofday(&t1, NULL);
	/*test code begin*/
	usleep(20);
	/*test code end*/
	gettimeofday(&t2, NULL);
	printf("%ld\n", t2.tv_usec - t1.tv_usec);
	return 0;
}
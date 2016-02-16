#include <unistd.h>
#include <sys/time.h>

int getDeltaTime( struct timeval &rTime1 )
{
	struct timeval time2;
	gettimeofday( &time2, NULL );
	double ret = (time2.tv_sec - rTime1.tv_sec) * 1000000 + (time2.tv_usec - rTime1.tv_usec);
	int nRet = (int)ret;
	return nRet;
}

#ifndef GETTIME_H
#define GETTIME_H

#include <sys/time.h>


#define rdtscll(val) \
        __asm__ __volatile__("rdtsc" : "=A" (val))

#ifdef __i386__
#define TIME(str, exp) { \
    long long _then, _now, best = 1000000000; \
    int i; \
    for (i=0; i<10; i++) { \
	    rdtscll(_then); \
	    exp; \
	    rdtscll(_now); \
	    if (_now - _then < best) { \
		best = _now - _then; \
	    } \
    } \
    printf(str " took %.1f cycles/sample\n", (double)best/(double)SIZE); \
}
#else
#define TIME(str, exp) { \
    double _then, _now; \
    _then = gettime(); \
    exp; \
    _now = gettime(); \
    printf(str " took %f us/sample\n", 1000000.0 * (_now-_then) / (double)SIZE); \
}
#endif

static inline double gettime()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (double)(tv.tv_usec * 1.0e-6) + (double)(tv.tv_sec);
}

#endif

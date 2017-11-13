/*
   To compile:
    gcc -fPIC -shared -o libtimemachine.so timemachine.c -ldl
   To use:
    SECONDS_OFFSET=14123 LD_PRELOAD=`pwd`/libtimemachine.so date

 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <time.h>
#include <stdlib.h> /*getenv, atol*/

static  time_t  (*real_time)(time_t *t) = NULL;
static  int     (*real_clock_gettime)(clockid_t clk_id, struct timespec *tp) = NULL;
static  long    *offz    = NULL;

long    offset()
{
    if ( ! offz )
    {
        offz = (long*) malloc( sizeof(long) );
        *offz = 0;
        char *var = getenv("SECONDS_OFFSET");
        if ( var )
            *offz = atol( var );
    }
    return *offz;
}

time_t time(time_t *t)
{
    if ( ! real_time ) /* Only look up once */
        real_time       =   dlsym( RTLD_NEXT, "time" );
    if ( ! real_time ) /* if we couldn't find it */
        return -1;
    /* call the real function */
    time_t  res         =   real_time(t);
    if ( t != NULL )
        t += offset();
    res += offset();
    return res;
}

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    if ( ! real_clock_gettime ) /* Only look up once */
        real_clock_gettime       =   dlsym( RTLD_NEXT, "clock_gettime" );
    if ( ! real_clock_gettime ) /* if we couldn't find it */
        return -1;
    int rv = real_clock_gettime( clk_id, tp );
    if (rv == 0)
        tp->tv_sec += offset();
    return rv;
}

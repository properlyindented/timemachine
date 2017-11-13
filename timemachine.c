/*
    20150325 Robert Martin-Legene
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
static  long    timemachine_offz    = 0;
static  int     timemachine_initialised = 0;

void    timemachine_init()
{
    char *timemachine_secs_offset;
    if ( timemachine_initialised )
        return;
    timemachine_initialised = 1;

    timemachine_secs_offset = getenv("SECONDS_OFFSET");
    if ( timemachine_secs_offset )
        timemachine_offz = atol( timemachine_secs_offset );

    if ( ! real_time )
        real_time           =   dlsym( RTLD_NEXT, "time" );
    if ( ! real_clock_gettime )
        real_clock_gettime  =   dlsym( RTLD_NEXT, "clock_gettime" );
}

time_t time(time_t *t)
{
    time_t  res;
    timemachine_init();
    if ( ! real_time ) /* if we couldn't find it */
        return -1;
    if ( t != NULL )
        t += timemachine_offz;
    /* call the real function */
    res =  real_time(t);
    return (res + timemachine_offz);
}

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    timemachine_init();
    /* if we couldn't find it */
    if ( ! real_clock_gettime )
        return -1;
    int rv = real_clock_gettime( clk_id, tp );
    if (rv != 0)
        return rv;
    if (rv == 0)
    tp->tv_sec += timemachine_offz;
    return rv;
}

# timemachine

Allows you to manipulate time in most (even 3rd party) programs on Linux.

time(2) and clock_gettime(2) are usually located in shared libraries, so
if your application isn't statically linked, then this work with the
program.

Tested on Debian.

To compile
    gcc -fPIC -shared -o libtimemachine.so timemachine.c -ldl

To use
    SECONDS_OFFSET=14123 LD_PRELOAD=`pwd`/libtimemachine.so date


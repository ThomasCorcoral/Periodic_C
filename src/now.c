#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

//gcc -Wall -std=c99 -g -o now now.c

int main (int argc, char * argv[])
{  
    /*
    struct timeval time;

    gettimeofday(&time, NULL);

    printf("time : %ld us\n", (time.tv_sec * 1000000 + time.tv_usec));
    */

    time_t nowtime = time( NULL );

    printf( "Date and local time : %s\n", ctime( & nowtime ) );

    return 0;
}
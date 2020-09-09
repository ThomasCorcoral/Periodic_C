#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

//gcc -Wall -std=c99 -g -o when when.c

int main (int argc, char * argv[])
{  
    if(argc < 2)
    {
        return 1;
    }

    time_t gettime = atoi(argv[1]);

    printf( "Date and local time : %s\n", ctime( & gettime ) );

    return 0;
}
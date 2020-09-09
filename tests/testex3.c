#include "message.h"
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
 

int main( int argc, char ** argv )
{

    printf("PID period : %i\n", check_period_pid());

    return 0;
}
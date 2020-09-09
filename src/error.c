#include "message.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

// Exercice 4

void error_common()
{
    printf("usage : ./periodic start period cmd [arg]...\nusage : ./periodic\n");
}

void error_args_num(int argc)
{
    printf("Bad arguments number, minimum 3 arguments are needed and you put %i arguments\n", argc-1);
    error_common();
}

void error_args_sem(char* endPtr, long value)
{
    printf( "Bad string format: %s - value == %ld\n", endPtr, value );
    error_common();
}
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

    // ERRORS MANGEMENT

    if(argc < 2)
    {
        return 1;
    }

    int  fd[2];

    if (pipe(fd) == -1)
    {
        printf("pipe failed\n");
        return 1;
    }

    int  pid;

    if ((pid = fork()) < 0)
    {
        printf("fork failed\n");
        return 2;
    }

    // PROGRAM

    if (pid == 0)
    {
    /* child */
        char* buf_read;

        close(fd[1]); /* close write side */

        buf_read = recv_string(fd[0]);

        printf("child reads : \"%s\"\n", buf_read);
        free(buf_read);

        close(fd[0]); /* close the pipe */
    }
    else
    {
        /* parent */
        char* buf_write;
        buf_write = calloc(strlen(argv[1])+1, sizeof(char));
        strcpy(buf_write, argv[1]);
        close(fd[0]); /* close read side */

        /* send some data into the pipe */
        int size = send_string(fd[1], buf_write);
        printf("parent write : \"%s\" with size : %i\n", buf_write, size);
        free(buf_write);

        close(fd[1]); /* close the pipe */
    }

    return 0;
}

/*
Bonjour, je suis entrain de débuter le projet en c et j'ai un petit soucis avec les premières fonctions de l'exercice 2. Quand je veux récupérer la chaine de caractères que j'ai envoyé dans mon pipe, ça ne marche pas Mais quand j'utilise valgrind ça marche. Est-ce symptomatique d'une erreur de ma part ? 
*/
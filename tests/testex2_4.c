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
        char** buf_read;

        close(fd[1]); /* close write side */

        buf_read = recv_argv(fd[0]);

        printf("Child reads : ");
        int y=0;

        close(fd[0]); /* close the pipe */

        while(buf_read[y] != NULL)
        {
            printf(" \"%s\" ", buf_read[y]);
            free(buf_read[y]);
            y++;
        }

        printf(" \"%s\" \n", buf_read[y]);

        free(buf_read[y]);
        free(buf_read);
    }
    else
    {
        /* parent */
        close(fd[0]); /* close read side */

        char** buf_write;
        buf_write = calloc(argc, sizeof(char*));

        for(int i = 0; i < argc-1; i++)
        {
            //printf("i : %i \n", i);
            buf_write[i] = calloc(strlen(argv[i+1])+1, sizeof(char));
            strcpy(buf_write[i], argv[i+1]);
        }

        buf_write[argc-1] = NULL;

        send_argv(fd[1], buf_write);

        //printf("\nWith size : %i\n", size);


        //free(buf_write);

        close(fd[1]); /* close the pipe */

        printf("parent write : ");
        int y = 0;
        
        while(buf_write[y] != NULL)
        {
            printf(" \"%s\" ", buf_write[y]);
            free(buf_write[y]);
            y++;
        }
        
        printf(" \"%s\" \n", buf_write[y]);
        free(buf_write[y]);
        free(buf_write);
    }

    return 0;
}

/*
Bonjour, je suis entrain de débuter le projet en c et j'ai un petit soucis avec les premières fonctions de l'exercice 2. Quand je veux récupérer la chaine de caractères que j'ai envoyé dans mon pipe, ça ne marche pas Mais quand j'utilise valgrind ça marche. Est-ce symptomatique d'une erreur de ma part ? 
*/
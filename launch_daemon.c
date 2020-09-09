#include "message.h"
#include "error.h"

#define _POSIX_SOURCE

#include <memory.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Fichier pour question bonus
FILE *flog;

/**
 * Initialisation du lanceur
 * Change le répertoire courant à "/"
 * Change le umask à 0
 * Ferme tous les descripteurs de fichiers standard
 * Ouvre le fichier daemon.flog
  */
void init()
{
    chdir("/");
    umask(0);
    close(0);
    close(1);
    close(2);

    // BONUS: To get a log
    // log daemon activity into a file
    flog = fopen("/tmp/daemon.flog","w+");
    int fd = fileno(flog);
    dup2(fd,1);
    dup2(fd,2);
}

/**
 * Termine le lanceur
 * Ferme le fichier daemon.flog
 */
void finish()
{
    fclose(flog);
}

/**
 * Vérifie le nombre d'arguments
 * Execute les arguments
 *
 * @param int argc             nombre d'éléments de la ligne de commande
 * @param const char *argv[]   tableau contenant les éléments de la ligne de commande
 */
void process(int argc, const char * argv[])
{
    if (argc < 2)
    {
        fprintf(flog,"Usage: launch_daemon /path/to/bin [arg1 arg2 ...]\n");
        exit(EXIT_FAILURE);
    }
    execvp(argv[0],(char * const *)argv);
    fprintf(flog,"Error: exec failed with %s",argv[0]);
}

/**
 * Fonction principale (main)
 */
int main (int argc, const char * argv[])
{
    pid_t pid1;
    pid_t pid2;

    init();

    pid1 = fork();
    if (pid1 != 0) {
        // Parent process wait its child
        wait(NULL);
    }

    else{ // Child process
        // Take the lead
        if( setsid() == -1)
        {
            fprintf(flog,"Erro: Fail to take the lead on the session\n");
            exit(EXIT_FAILURE);
        }

        pid2 = fork();
        if (pid2!=0) {
            // Child process exits
            exit(EXIT_SUCCESS);
        }
        // Grandchild process
        // We are now a daemon
        process(argc,argv+1);
    }

    finish();
}

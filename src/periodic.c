#include "message.h"
#include "error.h"

#define _POSIX_SOURCE
extern const char *SPECIAL_DELETE;

#include <memory.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>

int check_num(char* arg){
    char * endPtr;
    long value = strtol( arg, &endPtr, 10 ); 
    if (strcmp(endPtr,"") != 0){
        error_args_sem(endPtr, value);
        return -1;
    }
    return value;
}

/**
 * Fonction qui va analysé le second élément passé passé en paramètre
 * 
 * @param char* arg     second élément de argv
 * @param bool* plus    pointeur vers un booléen qui permet de savoir si le second argument est de la forme +xx
 * 
 * @return int      valeur de cet argument (on enleve le + si jamais)
 */
int check_secd_arg(char* arg, bool* plus){
    long value;
    if(arg[0] == '+'){
        char* copy;
        copy = calloc(strlen(arg), sizeof(char));
        for(int i = 1; i < strlen(arg); i++)
        {
            copy[i-1] = arg[i];
        }
        value = check_num(copy);
        free(copy);
        *plus = true;
    } else if(strcmp(arg, "now") == 0){
        value = (int) time(NULL);
    
    }else{
        value = check_num(arg);
    }
    return value;
}

/**
 * copie le tableau d'argv recu dans le main afin d'avoir uniquement
 * la commande et ces arguments
 * 
 * @param char** argv   arguments fournient au main
 * @param int    argc   nombre d'arguments fournient au main
 */
char** copy_tab(char ** argv, int argc)
{
    char** argstab;
    argstab = calloc(argc-2 , sizeof(char*));

    for(int i = 0; i < argc-3; i++){
        argstab[i] = calloc(strlen(argv[i+3])+1, sizeof(char));
        strcpy(argstab[i], argv[i+3]);
    }

    return argstab;
}

/**
 * Supprime un tableau de chaines de caractères
 * 
 * @param char** argstab    tableau d'arguments
 * @param int    size       taille du tableau
 */
void free_chartab(char ** argstab, int size)
{
    for(int i = 0; i < size-1; i++)
    {
        free(argstab[i]);
    }
    free(argstab);
}

/**
 * Ecrit une commande 
 * 
 * @param char** cmds   commande avec ses arguments à afficher
 * @param int size      taille de la commande 
 */
void print_cmds(char** cmds, int size, int index)
{
    for(int i = 0; i < size-1; i++)
    {
        printf("%d. %s ", index, cmds[i]);
    }
    printf("\n");
}

int main( int argc, char ** argv ){
    if(argc < 3 && argc != 1){ // Il faut minimum 4 arguments ou bien 1 seul
        error_args_num(argc);
        return 1;
    }

    // BONUS
    if (argc == 3 && (strcmp(argv[1],"delete") == 0))
    {
	// Recuperation du numero de commande a supprimer
	int cmd_num = check_num(argv[2]);
	pid_t pid = check_period_pid();
	kill(pid,SIGUSR1);
	struct stat sb;
	char path[] = "/tmp/period.fifo";
	if (stat(path,&sb) == -1)
	{
		printf("Pas de fichier");
	}
	int entreeTube;
	if((entreeTube = open(path, O_WRONLY)) == -1)
	{
		fprintf(stderr,"Impossible d'ouvrir l'entree du tube\n");
		exit(EXIT_FAILURE);
	}
	send_int(entreeTube,0);
	send_int(entreeTube,cmd_num);
	send_argv(entreeTube,(char **)&SPECIAL_DELETE);
	close(entreeTube);
	return EXIT_SUCCESS;
    }
    bool see = false;
    if(argc == 1){
        see = true;
    }

    bool plus = false;
    if(!see)
    {
        long start = check_secd_arg(argv[1], &plus);   // Vérification de l'heure de départ
        if(start == -1)
            return 2;

        if(plus)
            start += (int) time(NULL);

        long frequency = check_num(argv[2]);    // Vérification de la fréquence d'execution
        if(frequency == -1)
            return 3;

        char ** argstab;
        argstab = copy_tab(argv, argc);     // On stocke les arguments dans argstab. 
        // On envoie un signal à period
        pid_t pid = check_period_pid();
        kill(pid, SIGUSR1);
        // On prépare le tube
        struct stat sb;
        char path[] = "/tmp/period.fifo";
        if (stat(path, &sb) == -1) 
        {
            printf("Pas de fichier");
        }
        int entreeTube;
        if((entreeTube = open(path, O_WRONLY)) == -1) 
        {
            fprintf(stderr, "Impossible d'ouvrir l'entrée du tube nommé.\n");
            exit(EXIT_FAILURE);
        }
        // Envoie des informations dans le tube
        send_int(entreeTube, start);        // heure de départ
        send_int(entreeTube, frequency);    // fréquence
        send_argv(entreeTube, argstab);     // arguments
        // fermeture du tube
        close(entreeTube);
        // on free notre tableau d'arguments
        free_chartab(argstab, argc-2);
        
        return EXIT_SUCCESS;
    }
    else
    {
        pid_t pid = check_period_pid();
	if (pid == -1)
	{
		printf("No period process is running\n");
		exit(EXIT_FAILURE);
	}
        kill(pid, SIGUSR2);
        // On prépare le tube
        struct stat sb;
        char path[] = "/tmp/period.fifo";
        if (stat(path, &sb) == -1) 
        {
            printf("Pas de fichier");
        }
        int sortieTube;
        if((sortieTube = open (path, O_RDONLY)) == -1) 
        {
            fprintf(stderr, "Impossible d'ouvrir la sortie du tube nommé.\n");
            exit(EXIT_FAILURE);
        }
        // Reception des informations par le tube
        int size = recv_int(sortieTube);        // Taille de la liste
        int size_cmd;
        char** cmds;
        for(int i = 0; i < size; i++)
        {
            cmds = recv_argv(sortieTube, &size_cmd);     // arguments
            print_cmds(cmds, size_cmd, i);
            free(cmds);
        }
        
        // fermeture du tube
        close(sortieTube);
        return EXIT_SUCCESS;
    }
}

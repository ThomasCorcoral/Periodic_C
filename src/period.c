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

extern const char *SPECIAL_DELETE;
volatile sig_atomic_t flag = 0;

struct cmd
{
	long time;
	int freq;
	size_t size;
	char** str;
};

struct liste
{
	size_t size;
	struct cmd* cmds;
	size_t capacity;
};

/**
 * initialisation de la liste contenant les commandes
 *
 * @return struct liste     liste initialisée
 */
struct liste init_liste()
{
	struct liste li;
	li.size = 0;
	li.capacity = 1;
	li.cmds = calloc(li.capacity, sizeof(struct cmd));
	return li;
}

/**
 * Permet de créer une nouvelle commande
 *
 * @param int size      taille de la commande soit la commande + nb d'arguments
 * @param char** str    tableau de chaine de caractère contenant la commande
 *
 * @return struct cmd
 */
struct cmd new_cmd(int size, char** str)
{
	struct cmd new_cmd;
	new_cmd.freq = 0;
	new_cmd.time = 0;
	new_cmd.size = size;
	new_cmd.str = str;
	return new_cmd;
}

/**
 * Suppression d'une commande et libération de la mémoire
 *
 * @param struct cmd* cmds      commande à supprimer
 */
void free_cmd(struct cmd* cmds)
{
	for(int i = 0; i < cmds->size; i++)
	{
		free(cmds->str[i]);
	}
	free(cmds->str);
	cmds->size = 0;
	cmds->time = 0;
	cmds->freq = 0;
}

/**
 * suppresion d'une commande dans la liste via l'indice de la commande
 *
 * @param int 		i 			indice de la commande dans la liste
 * @param struct liste* 	liste des commandes
 */
void remove_cmd(struct liste* liste_cmd, int i)
{
	if (i < liste_cmd->size)
	{
		free_cmd(&liste_cmd->cmds[i]);
		for(int idx = i; idx < liste_cmd->size - 1; idx++)
		{
			liste_cmd->cmds[i] = liste_cmd->cmds[i+1];
			fprintf(stderr,"Command %d %s takes position %d\n",i+1 , liste_cmd->cmds[i+1].str[0] , i);
		}
		liste_cmd->size -= 1;
	}
}

/**
 * Ajout d'une commande dans une liste tout en prenant en compte
 * la capacité de la liste. La fonction gère également la mémoire
 * et peut faire croitre la taille de la liste dynamiquement
 *
 * @param struct liste *liste_cmd       liste à laquelle va s'ajouter la commande
 * @param struct cmd to_add             commande à ajouter
 */
void add_cmd(struct liste *liste_cmd, struct cmd to_add)
{
	if (strcmp(to_add.str[0],SPECIAL_DELETE) == 0)
	{
		int size1 = liste_cmd->size;
		remove_cmd(liste_cmd,to_add.freq);
		int size2 = liste_cmd->size;
		if ( (size1-size2) == 1)
			fprintf(stderr,"Command %d deleted\n",to_add.freq);
		else
			fprintf(stderr,"Error: Fail to delete command %d\n",to_add.freq);
		return;
	}
	if(liste_cmd->size == liste_cmd->capacity)
	{
		liste_cmd->capacity *= 2;	//O(1) amorti
		struct cmd* new_data = calloc(liste_cmd->capacity, sizeof(struct cmd));	// nouveau tableau
		memcpy(new_data, liste_cmd->cmds, liste_cmd->size * sizeof(struct cmd));	// Copie
		free(liste_cmd->cmds);	// Suppression de l'ancien
		liste_cmd->cmds = new_data;	// Copie du nouveau dans la structure
	}
	liste_cmd->cmds[liste_cmd->size] = to_add;
	liste_cmd->size++;
}


/**
 * Suppression d'une liste et libération de la mémoire
 *
 * @param struct liste* liste_cmd       liste à supprimer
 */
void free_liste(struct liste* liste_cmd)
{
	for(int i = 0; i < liste_cmd->size; i++)
	{
		free_cmd(&liste_cmd->cmds[i]);
	}
	free(liste_cmd->cmds);
	liste_cmd->capacity = 0;
	liste_cmd->size = 0;
}

/**
 * Fonction de test qui va créer 3 commandes à savoir
 * - history
 * - help
 * - ls -l
 *
 * @param struct liste *liste_cmd   liste qui a été initialisé avant à laquelle on ajoute les éléments cités ci-dessus
 */
void test(struct liste *liste_cmd)
{
	char** c1;
	c1 = calloc(2, sizeof(char*));
	c1[0] = calloc(8, sizeof(char));
	strcpy(c1[0], "history");
	struct cmd cm1 = new_cmd(1,c1);

	char** c2;
	c2 = calloc(2, sizeof(char*));
	c2[0] = calloc(5, sizeof(char));
	strcpy(c2[0], "help");
	struct cmd cm2 = new_cmd(1,c2);

	char** c3;
	c3 = calloc(3, sizeof(char*));
	c3[0] = calloc(3, sizeof(char));
	strcpy(c3[0], "ls");
	c3[1] = calloc(3, sizeof(char));
	strcpy(c3[1], "-l");
	struct cmd cm3 = new_cmd(2,c3);

	add_cmd(liste_cmd, cm1);
	add_cmd(liste_cmd, cm2);
	add_cmd(liste_cmd, cm3);
}

void secure_fclose(FILE *f)
{
	int ret = fclose(f);
	if(ret)
	{
		perror("fclose");
		exit(4); //=> Fin du programme
	}
}

/**
 * Ecriture du pid dans le fichier "/tmp/period.pid"
 */
int write_pid()
{
	char* file = "/tmp/period.pid";
	struct stat sb;
	if(stat(file, &sb) != -1) // Vérification de l'existance du fichier
	{
		fprintf(stderr, "Il y a déjà un fichier period.pid\n");
		exit(EXIT_FAILURE);
	}
	FILE *fp = fopen(file, "w");
	if (fp == NULL)
	{
		fprintf(stderr, "Failed to open the file %s\n", file);
		perror("fopen");
		return -1;
	}
	fprintf(fp, "%i", getpid());
	secure_fclose(fp);
	return 0;
}

/**
 * Redirection de stdout stderr
 */
void redirect_out()
{
	int d = open("/tmp/period.out",O_WRONLY | O_CREAT | O_TRUNC, 0666);
	int ret = dup2(d,STDOUT_FILENO);
	ret = close(d);
	if (ret != 0)
	{
		perror("close");
		exit(1);
	}
	d = open("/tmp/period.err",O_WRONLY | O_CREAT | O_TRUNC, 0666);
	ret = dup2(d,STDERR_FILENO);
	ret = close(d);
	if (ret != 0)
	{
		perror("close");
		exit(1);
	}
}

/**
 *	Compare les signaux reçu pour les flags
 *
 * @param int  flag       numéro du flag
 * @param int  SIGFLAG    signal
 */
int get_sigmatch(int flag, int SIGFLAG)
{
	if ( (SIGFLAG == SIGUSR1) && ((flag&1) == 1))
		return 1;
	else if ( (SIGFLAG == SIGUSR2) && (((flag>>1)&1) == 1))
		return 1;
	else if ( (SIGFLAG == SIGALRM) && (((flag>>2)&1) == 1))
		return 1;
	else if ( (SIGFLAG == SIGCHLD) && (((flag>>3)&1) == 1))
		return 1;
	else if ( (SIGFLAG == SIGINT || SIGFLAG == SIGQUIT  || SIGFLAG == SIGTERM) && (((flag>>4)&1) == 1))
		return 1;
	return 0;
}

/**
 * Fonction qui est appelée lorsqu'elle reçoit un des
 * signaux attendus à savoir SIGUSR1, SIGUSR2, SIGALRM, SIGCHLD, SIGINT, SIGTERM ou SIGQUIT
 *
 * @param int sig   signal que le programme reçoit
 */
void handler(int sig){
	if(sig == SIGUSR1){
		flag |= 1;
		//printf("SIGUSR1\n");
	}
	else if(sig == SIGUSR2){
		flag |= 2;
		//printf("SIGUSR2\n");
	}
	else if(sig == SIGALRM){
		flag |= 4;
		//printf("SIGALRM\n");
	}
	else if(sig == SIGCHLD){
		flag |= 8;
		//printf("SIGCHLD\n");
	}
	else if(sig == SIGINT || sig == SIGQUIT  || sig == SIGTERM){
		flag |= 16;
		//printf("SIGINT or SIGQUIT or SIGTERM\n");
	}
}

/**
 * Fonction principale (main)
 */
int main (int argc, const char * argv[])
{
	char * period_root = "/tmp/period/";
	char * dotout = ".out";
	char * doterr = ".err";
	char workingpath[256];
	int fdnull,fderr,fdout;

	time_t now;
	struct liste li = init_liste();     // Initialisation de la liste des commandes
	write_pid();    // Ecriture du pid dans le fichier "/tmp/period.pid"
	flag = 0;
	//redirect_out();   // redirection de stdout et stderr
	struct stat sb;
	char path[] = "/tmp/period.fifo";   // Chemin du tube
	if(stat(path, &sb) == -1) // Vérification de l'existance du tube
	{
		if(mkfifo(path, 0644) != 0)     // Création du tube
		{
			fprintf(stderr, "Impossible de créer le tube.\n");
			exit(EXIT_FAILURE);
		}
	}
	struct stat file_stat;

	if (stat("/tmp/period", &file_stat) < 0)
	{
		mkdir("/tmp/period", 0777);  // Permissions à vérifier, pas sûr
	}
	struct sigaction action;
	sigemptyset(&action.sa_mask);
	action.sa_handler = handler;    // fonction à appeler lors de la réception d'un des deux signals ci-dessous
	action.sa_flags = 0;
	sigaction(SIGUSR1, &action, NULL);  // Mise en place de l'écoute sur SIGUSR1
	sigaction(SIGUSR2, &action, NULL);  // Mise en place de l'écoute sur SIGUSR2
	sigaction(SIGALRM, &action, NULL);  // Mise en place de l'écoute sur SIGALRM
	sigaction(SIGCHLD, &action, NULL);  // Mise en place de l'écoute sur SIGCHLD
	sigaction(SIGINT, &action, NULL);   // Mise en place de l'écoute sur SIGINT
	sigaction(SIGQUIT, &action, NULL);  // Mise en place de l'écoute sur SIGQUIT
	sigaction(SIGTERM, &action, NULL);  // Mise en place de l'écoute sur SIGTERM


	pause();    // On attend un signal
	while(1)
	{


		if(get_sigmatch(flag,SIGUSR1))   // Pour SIGUSR1
		{
			flag &= 0xfe; // Disable flag
			// On prépare le tube
			struct stat sb;
			char path[] = "/tmp/period.fifo";
			if (stat(path, &sb) == -1){      // Vérification de l'existance du tube.
				fprintf(stderr, "Le tube n'existe pas.\n");
				exit(EXIT_FAILURE);
			}

			int sortieTube;
			if((sortieTube = open (path, O_RDONLY)) == -1) {
				fprintf(stderr, "Impossible d'ouvrir la sortie du tube nommé.\n");
				exit(EXIT_FAILURE);
			}
			// Reception des informations par le tube
			struct cmd cmds;
			cmds.time = recv_int(sortieTube);   // On récupère l'heure de départ
			cmds.freq = recv_int(sortieTube);   // On récupère la fréquence d'execution
			int size;
			cmds.str = recv_argv(sortieTube, &size);
			cmds.size = size;
			add_cmd(&li, cmds);
			close(sortieTube);
		}
		else if(get_sigmatch(flag,SIGUSR2))  // Pour SIGUSR2
		{
			flag &= 0xfd;
			int entreeTube;
			struct stat sb;
			char path[] = "/tmp/period.fifo";
			if (stat(path, &sb) == -1) {      // Vérification de l'existance du tube.
				fprintf(stderr, "Le tube n'existe pas.\n");
				exit(EXIT_FAILURE);
			}
			if((entreeTube = open(path, O_WRONLY)) == -1)
			{
				fprintf(stderr, "Impossible d'ouvrir l'entrée du tube nommé.\n");
				exit(EXIT_FAILURE);
			}

			//test(&li); // Permet de tester l'affichage en cas d'appelle de ./periodic sans arguments

			// Envoie des informations dans le tube
			send_int(entreeTube, li.size);        // Taille de la liste
			for(int i = 0; i < li.size; i++)
			{
				send_argv(entreeTube, li.cmds[i].str );     // arguments
			}
			// fermeture du tube
			close(entreeTube);

		}
		else if(get_sigmatch(flag,SIGALRM)) // Pour SIGALRM
		{
			flag &= 0xfb;
			now = time(NULL);
			for(int i = 0; i < li.size; i++)
			{
				if (li.cmds[i].time <= now) // /!\ Attention au signe potentiel problème
				{

					if (fork() == 0)
					{
						// Prepare stdint, stdout and stderr pour cette commande
						// Redirige stdin vers /dev/null
						fdnull = open("/dev/null",O_RDONLY);
						dup2(fdnull,0);

						// Redirige stdout vers /tmp/period/X.out
						sprintf(workingpath,period_root);
						sprintf(workingpath+strlen(workingpath),"%d",i+1);
						sprintf(workingpath+strlen(workingpath),dotout);
						fdout = open(workingpath,O_WRONLY|O_APPEND|O_CREAT,0644);
						dup2(fdout,1);

						// Redirige stderr vers /tmp/period/X.err
						memset(workingpath,0,strlen(workingpath));
						sprintf(workingpath,period_root);
						sprintf(workingpath+strlen(workingpath),"%d",i+1);
						sprintf(workingpath+strlen(workingpath),doterr);
						fderr = open(workingpath,O_WRONLY|O_APPEND|O_CREAT,0644);
						dup2(fderr,2);

						execvp(li.cmds[i].str[0],li.cmds[i].str);
						printf("Erreur d'execution de la commande %s\n", li.cmds[i].str[0]); // /!\ Redirection active donc le printf se fera sur /tmp/period/X.out
						exit(EXIT_FAILURE);
					}



					// Nous sommes sur le point d'executer une commande, il est temps de vérifer sa fréquence.
					if(li.cmds[i].freq > 0)
					{
						li.cmds[i].time = now + li.cmds[i].freq;
					}
					else // Si la frequence est nulle on supprime de notre liste.
					{
						remove_cmd(&li,i);
					}

				}
			}

		}
		else if(get_sigmatch(flag,SIGCHLD)){ // Pour SIGCHLD
			flag &= 0xf7;
			int status, exit_status;
			pid_t pid;

			fprintf(stderr, "SIGCHLD received \n");

			while((pid = waitpid(0,&status,WNOHANG)) != -1){
				if(pid != 0){
					exit_status = WEXITSTATUS(status);
					fprintf(stderr, "The child process %d ended with exit status %d\n", pid, exit_status);
				}
				else{
					break;
				}
			}
		}

		else if(get_sigmatch(flag,SIGINT)) // Pour SIGINT ou SIGTERM ou SIGQUIT
		{
			flag &= 0xef;

			// test :
			//fprintf(stderr, "Good bye...\n");

			// On supprime le fichier /tmp/period.pid
			if (remove("/tmp/period.pid") != 0){
				fprintf(stderr, "Impossible de supprimer le fichier period.pid\n");
			}

			// On libère l'espace mémoire alloué dynamiquement
			free_liste(&li);

			// On termine et élimine tous les processus créés restant
			kill(0,SIGTERM);

			int status, exit_status;
			pid_t pid;
			while((pid = waitpid(0,&status,WNOHANG)) != -1){
				if(pid != 0){
					exit_status = WEXITSTATUS(status);
					fprintf(stderr, "The child process %d ended with exit status %d\n", pid, exit_status);
				}
			}

			break;
		}
		else
		{
			fprintf(stderr, "Problème dans la reception du signal.\n");
		}

		// Parcours de la liste pour savoir combien de temps le programme va rentrer en pause.
		int setalarm = 0; // flag
		time_t delay = LONG_MAX;
		now = time(NULL);
		for(int i = 0; i < li.size; i++)
		{
			time_t tmp = li.cmds[i].time - now;
			// On cherche le plus petit délai
			if (tmp < delay)
			{
				delay = tmp;
				setalarm = 1;
			}
		}
		// Si l'on a trouvé une cmd on définit l'alarm en fonction du délai défini
		if (delay <= 0)
			flag = 4;
		else if (setalarm == 1)
		{
			alarm(delay);
			pause();
		}
		else if (li.size == 0)
		{
			pause();
		}
	}

	return 0;
}

#include "message.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>


#define BUFSIZE 256

// Bonus Exercice 15 :
const char * SPECIAL_DELETE = "++DELETE++";


// Exercice 2
struct chaine
{
    size_t size;
    char* str;
    size_t capacity;
};

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

int send_int(int fd, const int to_send)
{
    ssize_t size = write(fd, &to_send, sizeof(int));

    return (int) size;
}

int recv_int(int fd)
{
    int get;
    read(fd, &get, sizeof(int));

    return get;
}

int send_string(int fd, const char *str)
{
    int len;
    len = strlen(str) + 1;
    write(fd, &len, sizeof(int));  // longueur

    ssize_t size = write (fd, str, len);    // contenu

    if (size == len)        // tout s'est bien passé
    {
        return (int) size;      // renvoie la taille
    }
    else
    {
        return -1;      // Si erreur renvoie -1
    }
}


char *recv_string(int fd)
{
    int len;
    read(fd, &len, sizeof(int));
    char* str;
    str = calloc(len+1, sizeof(char));
    read(fd, str, len);
    return str;
}


int send_argv(int fd, char *argv[])
{
    int len = 0;
    while(argv[len] != NULL)
    {
        len++;
    }
    len++;

    write(fd, &len, sizeof(int));  // longueur

    int save;

    for(int i = 0; i < len-1; i++)
    {
        send_string(fd, argv[i]);
        save = i;
    }
    if (save == len)        // tout s'est bien passé
    {
        return (int) save;      // renvoie la taille
    }
    else
    {
        return -1;      // Si erreur renvoie -1
    }
}

char **recv_argv(int fd, int* size)
{
    read(fd, size, sizeof(int));

    char** str;
    str = calloc(*size, sizeof(char*));

    for(int i = 0; i < *size-1; i++)
    {
        str[i] = recv_string(fd);
    }
    str[*size-1] = NULL;
    return str;
}

// Exercice 3

void str_grow(struct chaine *self)
{
	self->capacity *= 2;	//O(1) amorti
	char *new_data = calloc(self->capacity, sizeof(char));	// nouveau tableau
	memcpy(new_data, self->str, self->size * sizeof(char));	// Copie
	free(self->str);	// Suppression de l'ancien
	self->str = new_data;	// Copie du nouveau dans la structure
}

void str_free(struct chaine *self)
{
    self->size = 0;
	self->capacity = 0;
	free(self->str);
}

int check_period_pid()
{
    FILE* fp;
    fp = fopen("/tmp/period.pid", "r");
    if(fp == NULL)
    {
        return -1;
    }
    else
    {
        struct chaine ch;
        ch.capacity = 1;
        ch.str = calloc(ch.capacity, sizeof(char));
        ch.size = 0;

        char* buf;
        buf = calloc(1, sizeof(char));
        int i = 0;

        while(feof(fp)==0)
        {
            if(ch.capacity == ch.size)
            {
                str_grow(&ch);
            }
            fread(buf, sizeof(char), 1, fp);
            ch.str[i] = buf[0];
            ch.size++;
            i++;
        }
        ch.str[i-1] = '\0';
        free(buf);
        int res = atoi(ch.str);
        //printf("res pid : %s\n", ch.str);
        str_free(&ch);
        int ret = fclose(fp);
        if(ret)
        {
            perror("fclose");
            exit(4); //=> Fin du programme
        }
        return res;
    }
}

#ifndef MESSAGE_H
#define MESSAGE_H

// Exercice 3

int send_int(int fd, const int to_send);
int recv_int(int fd);
int send_string(int fd, const char *str);
char *recv_string(int fd);
int send_argv(int fd, char *argv[]);
char **recv_argv(int fd, int* size);

// Exercice 4

int check_period_pid();

#endif
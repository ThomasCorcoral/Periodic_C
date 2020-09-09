#include <sys/stat.h>
#include <stdio.h> 


int main(int argc, char *argv[]) 
{ 
    struct stat file_stat; 
    
    if (stat("/tmp/loose",&file_stat) < 0)
    {
        printf("Le repertoire /tmp/period n\'existe pas!! :(\n");
    } 
    else 
    {
        printf("Le repertoire /tmp/period existe! :D\n");
    } 
    return 0; 
} 
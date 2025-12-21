#include <unistd.h>
#include "mini_lib.h"

#define DEBUG
#ifdef DEBUG
    #include <stdio.h>
#endif

const int BUF_SIZE 1024;
const char buffer[BUF_SIZE];
int ind = -1;

void mini_printf(char* msg){
    if (ind == -1){
        BUFFER = mini_calloc(sizeof(char), BUFF_SIZE);
        ind = 0;
    }

    size_t mess_lenght = sizeof(msg)/sizeof(char);
    int offset = -1;

    if((ind >= BUFF_SIZE -1) || (msg[ind] == '\n')){
        ssize_t ret = write(1, buffer, ind);
        if(ret < 0){
            error("Probleme d'ecriture");
        }

        ind = 0;
    }

    while(msg[ind] == '\n'){
        buffer[ind++] = msg[i];
        ind++;
    }

    // faire en sorte qu'un message plus long que la taille du buffer soit affiché avec l'offset
    //faire les 15 000 conditions pour la gestion d'erreur.
}
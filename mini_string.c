#include <unistd.h>
#include "mini_lib.h"

#define DEBUG
#ifdef DEBUG
    #include <stdio.h>
#endif

int BUF_SIZE = 1024;
char* buffer = NULL;
int ind = -1;

void mini_flush(){
    /* Envoie le buffer si il n'est pas vide */
    if (ind >0 && buffer != NULL){
        write(1, buffer, ind);
        ind = 0;
    }
}

void mini_printf(char* msg){
    if (ind == -1){
        buffer = (char *)mini_calloc(sizeof(char), BUF_SIZE);
        if (buffer == NULL) mini_exit(1);
        ind = 0;
    }
    int len = mini_strlen(msg);
    for (int i = 0; i < len; i++){
        buffer[ind] = msg[i];
        ind++;
        if ((ind == BUF_SIZE) || (msg[i] == '\n')){
            write(1, buffer, ind);
            ind = 0;
        }
    }
}
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

int mini_scanf(char* buffer, int size_buffer){
    mini_flush();
    int n = read(0, buffer, size_buffer -1);
    if (n < 0) mini_exit(1);
    buffer[n] = '\0';
    
    if(buffer[n-1] != '\n' && n == size_buffer - 1){
        char c;
        int ret;
        while(1){
            ret = read(0, &c, 1);
            /* On sort si erreur, fin de fichier, ou saut de ligne trouvé */
            if (ret <= 0 || c == '\n') {
                break;
            }
        }
    }
    
    if(n > 0 && buffer[n - 1] == '\n'){
        buffer[n - 1] = '\0';
        n--;
    }

    return n;
}
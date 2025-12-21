#include <unistd.h>
#include "mini_lib.h"

#define DEBUG
#ifdef DEBUG
    #include <stdio.h>
#endif

int BUF_SIZE 1024;
char buffer[BUF_SIZE];
int ind = -1;

void mini_printf(char* msg){
    if (ind == -1){
        buffer = mini_calloc(sizeof(char), BUF_SIZE);
        ind = 0;
    }
    for (int i = 0; i < _internal_strlen(msg); i++){

        buffer[ind] = msg[i];
        ind++;
    }
}
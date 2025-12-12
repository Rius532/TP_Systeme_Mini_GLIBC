#include "mini_lib.h"

#define DEBUG
#ifdef DEBUG
    #include <stdio.h>
#endif  //DEBUG

int main(){
    void* p1 = mini_calloc(10, 10);
    void* p2 = mini_calloc(5,5);

    #ifdef DEBUG
        printf("taille de la liste memoire : %d\n", nb_ptr_utilisee());
    #endif

    mini_free(p1);
    #ifdef DEBUG
        printf("taille de la liste memoire : %d\n", nb_ptr_utilisee());
    #endif

    mini_free(p2);
    #ifdef DEBUG
        printf("taille de la liste memoire : %d\n", nb_ptr_utilisee());
    #endif

    return 0;
}
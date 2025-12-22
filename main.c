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
    
    //TODO : vérifier que ça fait bien une réallocation en reprenant les pointeurs définis et libérés précédemments.
    void* p3 = mini_calloc(2,2);
    #ifdef DEBUG
        printf("taille de la liste memoire : %d\n", nb_ptr_utilisee());
    #endif
    mini_free(p3);
    
    mini_printf("Hello World !");

    mini_exit(0);
}
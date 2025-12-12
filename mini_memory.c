#include <unistd.h>
#include <stddef.h>
#include "mini_lib.h"

#define DEBUG
#ifdef DEBUG
    #include <stdio.h>
#endif  //DEBUG

void mini_exit(int status){
    _exit(status);
}

void error (char* message){
    printf("%s\n", message);
    mini_exit(1);
}

void mini_memset(void* ptr, int size_element, int number_element){
    for (int i = 0; i < size_element * number_element; i++){
        ((char*)ptr)[i] = 0;
    }
    #ifdef DEBUG
        printf("memset(%d, %d) : %p (memoire remise a 0)\n", size_element, number_element, ptr);
    #endif 
}

void* mini_calloc (int size_element, int number_element){
    
    void *ptr = sbrk(size_element * number_element);
    #ifdef DEBUG
        printf("sbrk(%d) : %p\n", size_element * number_element, ptr);
    #endif
    if (ptr == (void*) -1){ 
        error("Allocation impossible");
    }
    mini_memset(ptr, size_element, number_element);
    return ptr;
}

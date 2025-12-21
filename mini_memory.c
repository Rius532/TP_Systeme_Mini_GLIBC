#include <unistd.h>
#include <stddef.h>
#include "mini_lib.h"


#define DEBUG
#ifdef DEBUG
    #include <stdio.h>
#endif  //DEBUG

malloc_element* malloc_list = NULL;

void mini_exit(int status){
    _exit(status);
}

int _internal_strlen(char *s) {
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

void error(char* message){
    write(2, message + "\n", (_internal_strlen(message)+1));
    mini_exit(1);
}


void mini_memset(void* ptr, int size_element, int number_element){
    char* p = (char*)ptr;
    int total_size = size_element * number_element;
    for (int i = 0; i < total_size; i++){
        p[i] = '\0';
    }
    #ifdef DEBUG
        printf("memoire remise a 0 (%d, %d) : %p (mini_memset))\n", size_element, number_element, ptr);
    #endif 
}

void* mini_calloc (int size_element, int number_element){
    
    void *ptr = sbrk(size_element * number_element);

    #ifdef DEBUG
        printf("memoire allouee(%d) : %p\n", size_element * number_element, ptr);
    #endif

    if (ptr == (void*) -1){ 
        error("Allocation impossible");
    }
    
    malloc_element *current = malloc_list;
    while (current != NULL){
        if((current->status == LIBRE) && (current->size >= size_element * number_element)){
            current->status = UTILISE;
            mini_memset(ptr, size_element, number_element);            
            return current->ptr;
        }
        else{
            current = current->next_malloc;
        }
    }

    malloc_element* new_malloc = sbrk(size_element * number_element);    
    new_malloc->ptr = ptr;
    new_malloc->size = size_element * number_element;
    new_malloc->status = UTILISE;
    new_malloc->next_malloc = malloc_list;
    malloc_list = new_malloc;
    mini_memset(ptr, size_element, number_element);            
    
    return ptr;
}
 
void mini_free(void* ptr){
    malloc_element* current = malloc_list;
    while (current != NULL){
        if(current->ptr == ptr){
            current->status = LIBRE;
            #ifdef DEBUG
                printf("free(%p) : %p\n", ptr, current->ptr);
            #endif
            return;
        }
        else{
            current = current->next_malloc;
        }
    }
    error("Pointeur introuvable");
}   

int nb_ptr_libre(){
    int nb = 0;
    malloc_element* current = malloc_list;
    while (current != NULL){
        if(current->status == LIBRE){
            nb++;
        }
        current = current->next_malloc;
    }
    return nb;
}
int nb_ptr_utilisee(){
    int nb = 0;
    malloc_element* current = malloc_list;
    while (current != NULL){
        if(current->status == UTILISE){
            nb++;
        }
        current = current->next_malloc;
    }
    return nb;
}
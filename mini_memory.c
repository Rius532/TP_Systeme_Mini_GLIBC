#include <unistd.h>
#include <stddef.h>
#include "mini_lib.h"


#define DEBUG
#ifdef DEBUG
    #include <stdio.h>
#endif  //DEBUG

/* Variable globale */
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
    write(2, message, _internal_strlen(message));
    write(2, "\n", 1);
    mini_exit(1);
}


void mini_memset(void* ptr, int size_element, int number_element){
    char* p = (char*)ptr;
    int total_size = size_element * number_element;
    for (int i = 0; i < total_size; i++){
        p[i] = '\0';
    }
    #ifdef DEBUG
        printf("mini_memset (%d elements of %d bytes) at %p\n", number_element, size_element, ptr);
    #endif
}


void* mini_calloc(int size_element, int number_element){
    int total_size = size_element * number_element;
    
    if (total_size <= 0) return NULL;

    /* Réutilisation de bloc */
    malloc_element *current = malloc_list;
    while (current != NULL){
        /* Si le bloc est LIBRE et que sa taille est SUFFISANTE */
        if(current->status == 0 && current->size >= total_size){
            current->status = UTILISE; 
            mini_memset(current->ptr, size_element, number_element); // Reset mémoire
            
            #ifdef DEBUG
                printf("DEBUG: Reusing block at %p (size: %d)\n", current->ptr, (int)current->size);
            #endif

            return current->ptr;
        }
        current = current->next_malloc;
    }

    /* Allocation système si aucune réutilisation possible */
    
    /* Allocation de la structure */
    malloc_element* new_el = sbrk(sizeof(malloc_element));
    if (new_el == (void*)-1) {
        return NULL; 
    }

    /* Allocation des données */
    void *ptr_data = sbrk(total_size);
    if (ptr_data == (void*)-1) {
        return NULL;
    }

    /* Initialisation de la structure */
    new_el->ptr = ptr_data;
    new_el->size = total_size;
    new_el->status = UTILISE;
    
    /* Insertion au début */
    new_el->next_malloc = malloc_list;
    malloc_list = new_el;

    /* Initialisation à 0*/
    mini_memset(ptr_data, size_element, number_element);

    #ifdef DEBUG
        printf("DEBUG: Allocated new block at %p (size: %d)\n", ptr_data, total_size);
    #endif
    
    return ptr_data;
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
    error("Invalid pointer freed");
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
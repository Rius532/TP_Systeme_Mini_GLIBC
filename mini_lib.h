#include <stddef.h>

#ifndef MINI_LIB_H
#define MINI_LIB_H

typedef struct malloc_element {
    void* ptr;              // Pointeur vers la zone utilisable
    size_t size;            // Taille demandée par l'utilisateur
    int status;             // 0: LIBRE, 1: UTILISÉ
    struct malloc_element* next_malloc; // Chaînage
} malloc_element;

extern malloc_element* malloc_list; // Variable globale

void* mini_calloc (int size_element, int number_element);

void mini_memset(void* ptr, int size_element, int number_element);

void mini_exit(int status);

#endif // MINI_LIB_H


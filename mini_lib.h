#include <stddef.h>

#ifndef MINI_LIB_H
#define MINI_LIB_H

typedef enum status{
    LIBRE = 0,
    UTILISE = 1
} Status;

typedef struct malloc_element {
    void* ptr;              // Pointeur vers la zone utilisable
    size_t size;            // Taille demandée par l'utilisateur
    Status status;             // 0: LIBRE, 1: UTILISÉ
    struct malloc_element* next_malloc; // Chaînage
} malloc_element;

extern malloc_element* malloc_list; // Variable globale

extern void mini_exit(int status);

int mini_strlen(char* s);

extern void error (char* message);

void mini_memset(void* ptr, int size_element, int number_element);

extern void* mini_calloc (int size_element, int number_element);

void mini_free(void* ptr);

int nb_ptr_libre();

int nb_ptr_utilisee();

void mini_flush();

void mini_exit_string();

void mini_printf(char* msg);

int mini_scanf(char* buffer, int size_buffer);

#endif // MINI_LIB_H


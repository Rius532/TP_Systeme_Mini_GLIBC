#include <stddef.h>

#ifndef MINI_LIB_H
#define MINI_LIB_H

typedef enum status
{
    LIBRE = 0,
    UTILISE = 1
} Status;

typedef struct malloc_element
{
    void *ptr;                          // Pointeur vers la zone utilisable
    size_t size;                        // Taille demandée par l'utilisateur
    Status status;                      // 0: LIBRE, 1: UTILISÉ
    struct malloc_element *next_malloc; // Chaînage
} malloc_element;

extern malloc_element *malloc_list; // Variable globale

extern void mini_exit(int status);

int mini_strlen(char *s);

extern void error(char *message);

void mini_memset(void *ptr, int size_element, int number_element);

extern void *mini_calloc(int size_element, int number_element);

void mini_free(void *ptr);

int nb_ptr_libre();

int nb_ptr_utilisee();

void mini_flush();

void mini_exit_string();

void mini_printf(char *msg);

int mini_scanf(char *buffer, int size_buffer);

int mini_strcpy(char *s, char *d);

int mini_strcmp(char *s1, char *s2);

void int_to_string(int n, char *buffer);

void mini_perror(char *message);

typedef struct
{
    int fd;
    void *buffer_read;
    void *buffer_write;
    int ind_read;
    int ind_write;
} MYFILE;

MYFILE *mini_fopen(char *file, char mode);

int mini_fwrite(void *buffer, int size_element, int number_element, MYFILE *file);

int mini_fread(void *buffer, int size_element, int number_element, MYFILE *file);

int mini_fflush(MYFILE *file);

int mini_fwrite(void *buffer, int size_element, int number_element, MYFILE *file);

int mini_fclose(MYFILE *file);

int mini_fgetc(MYFILE *file);

int mini_fputc(MYFILE *file, char c);

void mini_putchar(char c);

void mini_putstr(char *str);

void mini_putnbr(long n);

int mini_putnbr_octal(char *str);

char *get_full_path(char *dir, char *filename);

#endif // MINI_LIB_H

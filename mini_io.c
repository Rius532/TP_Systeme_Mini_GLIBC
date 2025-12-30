#include <unistd.h>
#include <fcntl.h>      /* Indispensable pour O_RDONLY, O_CREAT... */
#include <sys/types.h>
#include <sys/stat.h>   /* Pour les permissions (0644) */
#include "mini_lib.h"

int IOBUFFER_SIZE = 2048;

MYFILE* mini_fopen(char* file, char mode){
    int fd = -1;
    int flags = 0;

    // Permissions par défaut : rw-r--r--
    mode_t permissions = 0644;

    // gestion des modes en flags système
    if (mode == 'r') flags = O_RDONLY;
    else if (mode == 'w') flags = O_WRONLY | O_CREAT | O_TRUNC;
    else if (mode == 'b') flags = O_RDWR | O_CREAT;
    else if (mode == 'a') flags = O_WRONLY | O_CREAT | O_APPEND;
    else return NULL;

    // Appel system open
    if (flags & O_CREAT){
        fd = open(file, flags, permissions);
    }
    else{
        fd = open(file, flags);
    }
    if (fd == -1) return NULL;

    // Allocation de la strucutre MYFILE
    MYFILE* stream = (MYFILE*) mini_calloc(sizeof(MYFILE), 1);
    if (stream == NULL) {
        close(fd); /* Ne pas laisser un descripteur ouvert si l'alloc échoue */
        return NULL;
    }
    
    // Initialisation de la structure
    stream->fd = fd;
    stream->ind_read = -1;
    stream -> ind_write = -1;
    stream->buffer_read = NULL; // mini_calloc(sizeof(char), IOBUFFER_SIZE);
    stream->buffer_write = NULL; //mini_calloc(sizeof(char), IOBUFFER_SIZE);
    

    return stream;
}

int mini_fread(void* buffer, int size_element, int number_element, MYFILE* file) {
    if (file == NULL || buffer == NULL) return -1;

    int total_bytes_needed = size_element * number_element;
    int bytes_copied = 0;
    char* user_buffer = (char*)buffer; 

    // Allocation du buffer au 1er appel
    if (file->buffer_read == NULL) {
        file->buffer_read = mini_calloc(1, IOBUFFER_SIZE);
        if (file->buffer_read == NULL) return -1;
        file->ind_read = -1; 
    }

    while (bytes_copied < total_bytes_needed) {
        if (file->ind_read == -1 || file->ind_read == IOBUFFER_SIZE) {
            
            // On lit (tjs) IOBUFFER_SIZE éléments
            int n = read(file->fd, file->buffer_read, IOBUFFER_SIZE);
            
            if (n == -1) return -1;
            if (n == 0) break;      // Fin de fichier réelle

            file->ind_read = 0; // Reset l'index au début 
        }

        // Copie des données du buffer interne
        char* internal_buf = (char*)file->buffer_read;
        user_buffer[bytes_copied] = internal_buf[file->ind_read];
        
        bytes_copied++;
        file->ind_read++;
    }

    return bytes_copied; /* Retourne le nombre d'octets lus (ou à adapter selon le return voulu) [cite: 91] */
}

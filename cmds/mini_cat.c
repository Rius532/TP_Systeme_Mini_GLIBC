#include "mini_lib.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        mini_printf("Usage: ");
        mini_printf(argv[0]);
        mini_printf(" <filename1> ...\n");
        mini_exit(1);
    }

    MYFILE* stdout_stream = mini_calloc(sizeof(MYFILE), 1);
    if (stdout_stream == NULL) mini_exit(1);
    stdout_stream->fd = 1; 
    stdout_stream->ind_write = 0;
    stdout_stream->ind_read = -1;

    for (int i = 1; i < argc; i++) {
        MYFILE* f = mini_fopen(argv[i], 'r');
        if (f == NULL) {
            mini_perror("Erreur ouverture fichier");
            continue; // on passe au fichier suivant
        }

        // Lecture et écriture caractère par caractère 
        int c;
        while ((c = mini_fgetc(f)) != -1) {
            if (mini_fputc(stdout_stream, (char)c) == -1) {
                mini_perror("Erreur ecriture stdout");
                mini_fclose(f);
                break;
            }
        }   
        mini_fclose(f);
    }
    mini_fclose(stdout_stream);
    mini_exit(0);
}
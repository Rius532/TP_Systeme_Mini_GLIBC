#include "mini_lib.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        mini_printf("Usage: ");
        mini_printf(argv[0]);
        mini_printf(" <file1> <file2> ...\n");
        mini_exit(1);
    }

    for (int i = 1; i < argc; i++) {
        char* filename = argv[i];
        
        MYFILE* f = mini_fopen(filename, 'w');
        if (f == NULL) {
            mini_perror("Erreur: impossible de vider le fichier");
            mini_printf(filename);
            mini_printf("\n");
            mini_exit(1);
        } 
        else {
            mini_fclose(f);            
            mini_printf(filename);
            mini_printf(" : fichier remis a zero (clean).\n");
        }
    }
    mini_exit(0);
}
#include "mini_lib.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        mini_printf("Usage: ");
        mini_printf(argv[0]);
        mini_printf(" <filename>\n");
        mini_exit(1);
    }
    char* filename = argv[1];
    MYFILE* f = mini_fopen(filename, 'b');
    if (f == NULL) {
        mini_perror("Erreur mini_touch");
        mini_exit(1);
    }
    mini_fclose(f);
    mini_exit(0);
}
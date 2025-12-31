#include "mini_lib.h"

int main(int argc, char* argv[]) {
    if(argc != 3) {
        mini_printf("Usage: ");
        mini_printf(argv[0]);
        mini_printf(" <source> <destination>\n");
        mini_exit(1);
    }
    MYFILE* source = mini_fopen(argv[1], 'r');
    if(source == NULL) {
        mini_perror("Erreur ouverture source");
        mini_fclose(source);
        mini_exit(1);
    }
    MYFILE* destination = mini_fopen(argv[2], 'w');
    if(destination == NULL) {
        mini_perror("Erreur ouverture destination");
        mini_fclose(destination);
        mini_exit(1);
    }
    int c;
    while((c = mini_fgetc(source)) != -1) {
        if(mini_fputc(destination, c) == -1) {
            mini_perror("Erreur ecriture destination");
            break;
        }
    }
    mini_fclose(source);
    mini_fclose(destination);
    mini_exit(0);
}
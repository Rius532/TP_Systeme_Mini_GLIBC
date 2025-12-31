#include "mini_lib.h"

int my_atoi(char* str) {
    int res = 0;
    int i = 0;
    while (str[i] >= '0' && str[i] <= '9') {
        res = res * 10 + (str[i] - '0');
        i++;
    }
    return res;
}

int main(int argc, char **argv) {
    int n_lines_to_print = 10;
    int current_arg = 1;

    if (argc > 1 && mini_strcmp(argv[1], "-n") == 0) {
        if (argc < 3) {
            mini_printf("Option -n necessite un nombre.\n");
            mini_exit(1);
        }
        n_lines_to_print = my_atoi(argv[2]);
        current_arg = 3;
    }

    if (current_arg >= argc) {
        mini_printf("Usage: ./mini_head [-n lines] <file>\n");
        mini_exit(1);
    }

    // préparation de la sortie standard stdout
    MYFILE* stdout_stream = mini_calloc(sizeof(MYFILE), 1);
    stdout_stream->fd = 1; 
    stdout_stream->ind_write = 0;
    stdout_stream->ind_read = -1;

    // traitement des fichiers
    for (int i = current_arg; i < argc; i++) {
        MYFILE* f = mini_fopen(argv[i], 'r');
        if (f == NULL) {
            mini_perror("Erreur ouverture");
            continue;
        }

        // On affiche le nom du fichiers s'il y en a plusieurs
        if (argc - current_arg > 1) {
            mini_printf("==> ");
            mini_printf(argv[i]);
            mini_printf(" <==\n");
        }

        int c;
        int lines_read = 0;
        while (lines_read < n_lines_to_print && (c = mini_fgetc(f)) != -1) {
            mini_fputc(stdout_stream, (char)c);
            if (c == '\n') {
                lines_read++;
            }
        }
        mini_fclose(f);
        if (i < argc - 1) mini_fputc(stdout_stream, '\n');
    }
    mini_fclose(stdout_stream);
    mini_exit(0);
}
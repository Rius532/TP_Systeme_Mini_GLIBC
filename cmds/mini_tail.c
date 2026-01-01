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
        mini_printf("Usage: ./mini_tail [-n lines] <file>\n");
        mini_exit(1);
    }

    MYFILE* stdout_stream = mini_calloc(sizeof(MYFILE), 1);
    stdout_stream->fd = 1; 
    stdout_stream->ind_write = 0;
    stdout_stream->ind_read = -1;

    for (int i = current_arg; i < argc; i++) {
        char* filename = argv[i];

        // Compter les lignes
        MYFILE* f = mini_fopen(filename, 'r');
        if (f == NULL) {
            mini_perror("Erreur ouverture");
            continue;
        }

        int total_lines = 0;
        int c;
        while ((c = mini_fgetc(f)) != -1) {
            if (c == '\n') total_lines++;
        }
        mini_fclose(f); 

        int start_line = total_lines - n_lines_to_print;
        if (start_line < 0) start_line = 0;
        f = mini_fopen(filename, 'r'); // on reouvre au debut
        int current_line = 0;
        if (argc - current_arg > 1) {
            mini_printf("==> "); mini_printf(filename); mini_printf(" <==\n");
        }

        while ((c = mini_fgetc(f)) != -1) {
            if (current_line >= start_line) {
                mini_fputc(stdout_stream, (char)c);
            }
            if (c == '\n') current_line++;
        }
        mini_fclose(f);
        if (i < argc - 1) mini_fputc(stdout_stream, '\n');
    }
    mini_fclose(stdout_stream);
    mini_exit(0);
}
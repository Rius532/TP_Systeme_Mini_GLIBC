#include "mini_lib.h"

#define MAX_LINE_SIZE 4096

int has_pattern(char* line, char* pattern) {
    if (pattern == NULL || *pattern == '\0') return 1;
    int i = 0;
    while (line[i] != '\0') {
        int j = 0;
        while (pattern[j] != '\0' && line[i + j] == pattern[j]) {
            j++;
        }
        if (pattern[j] == '\0') return 1;        
        i++;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        mini_printf("Usage: ./mini_grep <pattern> <filename>\n");
        mini_exit(1);
    }
    char* pattern = argv[1];
    char* filename = argv[2];
    MYFILE* f = mini_fopen(filename, 'r');   
    if (f == NULL) {
        mini_perror("Erreur ouverture");
    }
    char line_buffer[MAX_LINE_SIZE];
    int index = 0;
    int c;

    while ((c = mini_fgetc(f)) != -1) {
        if (c == '\n' || index >= MAX_LINE_SIZE - 1) {
            line_buffer[index] = '\0';            
            if (has_pattern(line_buffer, pattern)) {
                if (argc > 3) {
                    mini_printf(filename);
                    mini_printf(":");
                }
                mini_printf(line_buffer);
                mini_printf("\n");
            }
            index = 0;
        } else {
            line_buffer[index] = (char)c;
            index++;
        }
    }
    if (index > 0) {
        line_buffer[index] = '\0';
        if (has_pattern(line_buffer, pattern)) {
            if (argc > 3) {
                mini_printf(filename);
                mini_printf(":");
            }
            mini_printf(line_buffer);
            mini_printf("\n");
        }
    }
    mini_fclose(f);
mini_exit(0);
}
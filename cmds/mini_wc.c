#include "mini_lib.h"

/* Helper pour détecter les séparateurs */
int is_separator(int c) {
    return (c == ' ' || c == '\t' || c == '\n');
}

int main(int argc, char **argv) {
    if (argc < 2) {
        mini_printf("Usage: ./mini_wc <filename>\n");
        mini_exit(1);
    }

    char* filename = argv[1];
    MYFILE* f = mini_fopen(filename, 'r');
    if (f == NULL) {
        mini_perror("Erreur ouverture");
        mini_exit(1);
    }

    int count_lines = 0;
    int count_words = 0;
    int count_bytes = 0;
    
    int in_word = 0; // état : 0 = hors d'un mot, 1 = dans un mot 
    int c;

    while ((c = mini_fgetc(f)) != -1) {
        if (c == '\0') break;
        
        count_bytes++; // Compte chaque octets lu

        // Gestion des lignes
        if (c == '\n') {
            count_lines++;
        }

        // Gestion des mots 
        if (is_separator(c)) {
            in_word = 0; 
        } 
        else if (in_word == 0) {
            // caractère normal ET hors d'un mot => nv mot
            in_word = 1;
            count_words++;
        }
        // Dans un mot => rien
    }

    mini_fclose(f);

    char buffer[32];
    
    int_to_string(count_lines, buffer);
    mini_printf(buffer);
    mini_printf("\t");

    int_to_string(count_words, buffer);
    mini_printf(buffer);
    mini_printf("\t");

    int_to_string(count_bytes, buffer);
    mini_printf(buffer);
    mini_printf("\t");

    mini_printf(filename);
    mini_printf("\n");

    mini_exit(0);
}
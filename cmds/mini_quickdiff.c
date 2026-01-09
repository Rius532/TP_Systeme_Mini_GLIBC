#include "mini_lib.h"

#define BUFFER_SIZE 1024

char *get_line(MYFILE *file)
{
    char *buffer = mini_calloc(1, BUFFER_SIZE);
    if (buffer == NULL)
        return NULL;
    int c;
    int i = 0;
    while (i < BUFFER_SIZE - 1 && (c = mini_fgetc(file)) != -1 && c != '\n')
    {
        buffer[i++] = c;
    }

    // Gestion EOF
    if ((i == 0) && c == -1)
    {
        mini_free(buffer);
        return NULL;
    }

    buffer[i] = '\0';
    return buffer;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        mini_printf("Usage: ./mini_quickdiff <file1> <file2>\n");
        mini_exit(1);
    }

    MYFILE *file1 = mini_fopen(argv[1], 'r');
    MYFILE *file2 = mini_fopen(argv[2], 'r');

    if (file1 == NULL || file2 == NULL)
    {
        mini_perror("Error opening file");
        mini_exit(1);
    }
    char *buffer1;
    char *buffer2;
    int n = 0;
    while (1)
    {
        buffer1 = get_line(file1);
        buffer2 = get_line(file2);
        n++;

        // Les deux fichiers finissent en même temps
        if (buffer1 == NULL && buffer2 == NULL)
            break;

        // Les deux fichiers ont des longueurs différentes
        if (buffer1 == NULL || buffer2 == NULL)
        {
            mini_printf("Files are not equal (length mismatch) at line ");
            mini_flush();
            mini_putnbr(n);
            mini_printf("\n");
            if (buffer1)
                mini_free(buffer1);
            if (buffer2)
                mini_free(buffer2);
            mini_exit(1);
        }

        // comparaison du contenu d'une ligne
        if (mini_strcmp(buffer1, buffer2) != 0)
        {
            mini_printf("line ");
            mini_flush();
            mini_putnbr((long)n);
            mini_printf(":\n> ");
            mini_printf(buffer1);
            mini_printf("\n< ");
            mini_printf(buffer2);
            mini_printf("\n\n");
        }
        mini_free(buffer1);
        mini_free(buffer2);
    }
    mini_fclose(file1);
    mini_fclose(file2);
    return 0;
}

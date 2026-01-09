#include "mini_lib.h"
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        mini_printf("Usage: ");
        mini_printf(argv[0]);
        mini_printf(" <mode> <filename>\n");
        mini_exit(1);
    }
    int mode = mini_putnbr_octal(argv[1]);
    if (mode == -1)
        mini_exit(1); // chiffre > 7
    char *filename = argv[2];

    // Appel système chmod
    if (chmod(filename, mode) == -1)
    {
        mini_perror("Error chmod");
        mini_exit(1);
    }

    return 0;
}

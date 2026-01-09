#include "mini_lib.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        mini_printf("Usage: ");
        mini_printf(argv[0]);
        mini_printf(" <source> <destination>\n");
        mini_exit(1);
    }
    if (symlink(argv[1], argv[2]) == -1)
    {
        mini_perror("Error symlink");
        mini_exit(1);
    }
    return 0;
}

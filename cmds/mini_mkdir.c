#include "mini_lib.h"
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        mini_printf("Usage: ./mini_mkdir <directory_name>\n");
        mini_exit(1);
    }

    if (mkdir(argv[1], 0777) == -1)
    {
        mini_perror("Error mkdir");
        mini_exit(1);
    }

    mini_exit(0);
}
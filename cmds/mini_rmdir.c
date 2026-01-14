#include "mini_lib.h"
#include <sys/stat.h>
#include <unistd.h> // unlink et lstat

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        mini_printf("Usage: ./mini_rmdir <directory>\n");
        mini_exit(1);
    }

    char *directory = argv[1];
    struct stat st;

    if (lstat(directory, &st) == -1)
    {
        mini_perror("Error rm (stat)");
        mini_exit(1);
    }

    if (S_ISDIR(st.st_mode))
    {
        if (rmdir(directory) == -1)
        {
            mini_perror("Error rmdir : directory not empty or permission denied");
            mini_exit(1);
        }
    }
    else
    {
        mini_printf("rm: cannot remove '");
        mini_printf(directory);
        mini_printf("': Is not a directory\n");
        mini_exit(1);
    }
    mini_exit(0);
}

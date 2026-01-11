#include <mini_lib.h>
#include <sys/stat.h>
#include <unistd.h> // unlink et lstat

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        mini_printf("Usage: ./mini_rm <file>\n");
        mini_exit(1);
    }

    char *file = argv[1];
    struct stat st;
    if (lstat(file, &st) == -1)
    {
        mini_perror("Error rm (stat)");
        mini_exit(1);
    }

    if (S_ISDIR(st.st_mode))
    {
        mini_printf("rm: cannot remove '");
        mini_printf(file);
        mini_printf("': Is a directory\n");
        mini_exit(1);
    }
    else if (S_ISREG(st.st_mode))
    {
        if (unlink(file) == -1)
        {
            mini_perror("Error rm (unlink)");
            mini_exit(1);
        }
    }
    else
    {
        mini_printf("rm: cannot remove '");
        mini_printf(file);
        mini_printf("': Is not a regular file\n");
        mini_exit(1);
    }

    mini_exit(0);
}

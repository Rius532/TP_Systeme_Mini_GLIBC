#include "mini_lib.h"
#include <unistd.h>
#include <sys/stat.h>

extern int rename(const char *oldpath, const char *newpath);

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        mini_printf("Usage: ./mini_mv <source> <destination>\n");
        mini_exit(1);
    }

    char *source = argv[1];
    char *dest = argv[2];
    char *final_path = dest;
    int must_free = 0;

    struct stat st;

    // 1. Vérification si la destination est un dossier
    if (lstat(dest, &st) != -1 && S_ISDIR(st.st_mode))
    {
        // pour extraire le nom du fichier source (gestion de a/b/fichier)
        char *filename = source;
        int i = 0;
        int last_slash = -1;
        while (source[i])
        {
            if (source[i] == '/')
                last_slash = i;
            i++;
        }
        if (last_slash != -1)
            filename = source + last_slash + 1;

        // On construit destination/fichier
        final_path = get_full_path(dest, filename);
        if (!final_path)
        {
            mini_perror("Error memory");
            mini_exit(1);
        }
        must_free = 1;
    }

    if (rename(source, final_path) == -1)
    {
        mini_perror("Error rename");
        if (must_free)
            mini_free(final_path);
        mini_exit(1);
    }

    if (must_free)
        mini_free(final_path);
    return 0;
}
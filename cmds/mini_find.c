#include "mini_lib.h"
#include <sys/stat.h>
#include <unistd.h>

int mini_fnmatch(char *pattern, char *string)
{
    if (*pattern == '\0' && *string == '\0')
        return 1;

    if (*pattern == '*')
    {
        if (mini_fnmatch(pattern + 1, string))
            return 1;

        if (*string != '\0' && mini_fnmatch(pattern, string + 1))
            return 1;

        return 0;
    }

    if (*string != '\0' && (*pattern == '?' || *pattern == *string))
    {
        return mini_fnmatch(pattern + 1, string + 1);
    }

    return 0;
}

void do_find(char *current_path, char *pattern_name)
{
    MINI_DIR *dir;
    struct mini_dirent *entry;
    struct stat st;

    // 1. Ouvrir le dossier
    dir = mini_opendir(current_path);
    if (!dir)
    {
        mini_perror("find: access denied");
        return;
    }

    while ((entry = mini_readdir(dir)) != NULL)
    {
        if (mini_strcmp(entry->d_name, ".") == 0 ||
            mini_strcmp(entry->d_name, "..") == 0)
            continue;

        char *full_path = get_full_path(current_path, entry->d_name);
        if (!full_path)
            continue;

        int match = 1;
        if (pattern_name != NULL)
        {
            match = mini_fnmatch(pattern_name, entry->d_name);
        }

        if (match)
        {
            mini_printf(full_path);
            mini_printf("\n");
        }

        if (lstat(full_path, &st) != -1)
        {
            if (S_ISDIR(st.st_mode))
            {
                do_find(full_path, pattern_name); // récursif
            }
        }

        mini_free(full_path);
    }

    mini_closedir(dir);
}

int main(int argc, char **argv)
{
    char *start_path = "."; // chemin de départ par défaut
    char *pattern = NULL;

    int i = 1;
    while (i < argc)
    {
        // Gestion argument -name
        if (mini_strcmp(argv[i], "-name") == 0)
        {
            if (i + 1 < argc)
            {
                pattern = argv[i + 1];
                i++;
            }
            else
            {
                mini_printf("find: missing argument to `-name'\n");
                mini_exit(1);
            }
        }
        else if (argv[i][0] != '-')
        {
            start_path = argv[i];
        }
        i++;
    }

    do_find(start_path, pattern);

    return 0;
}
#include "mini_lib.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

void print_file_type(mode_t mode)
{
    if (S_ISDIR(mode))
        mini_putchar('d');
    else if (S_ISLNK(mode))
        mini_putchar('l');
    else if (S_ISCHR(mode))
        mini_putchar('c');
    else if (S_ISBLK(mode))
        mini_putchar('b');
    else if (S_ISFIFO(mode))
        mini_putchar('p');
    else if (S_ISSOCK(mode))
        mini_putchar('s');
    else
        mini_putchar('-');
}

void print_permissions(mode_t mode)
{
    print_file_type(mode);
    mini_putchar((mode & S_IRUSR) ? 'r' : '-');
    mini_putchar((mode & S_IWUSR) ? 'w' : '-');
    mini_putchar((mode & S_IXUSR) ? 'x' : '-');
    mini_putchar((mode & S_IRGRP) ? 'r' : '-');
    mini_putchar((mode & S_IWGRP) ? 'w' : '-');
    mini_putchar((mode & S_IXGRP) ? 'x' : '-');
    mini_putchar((mode & S_IROTH) ? 'r' : '-');
    mini_putchar((mode & S_IWOTH) ? 'w' : '-');
    mini_putchar((mode & S_IXOTH) ? 'x' : '-');
}

void print_file_info(char *full_path, char *display_name)
{
    struct stat st;

    // Utilisation de lstat pour voir les liens symboliques eux-mêmes
    if (lstat(full_path, &st) == -1)
    {
        mini_perror("Error stat");
        return;
    }

    // Permissions
    print_permissions(st.st_mode);
    mini_putstr("  ");

    // Liens
    mini_putnbr((long)st.st_nlink);
    mini_putstr("  ");

    // UID / GID
    mini_putnbr((long)st.st_uid);
    mini_putstr("  ");
    mini_putnbr((long)st.st_gid);
    mini_putstr("  ");

    // Taille
    mini_putnbr((long)st.st_size);
    mini_putstr("  ");

    // Date (Timestamp brut)
    char date[200];
    strftime(date, sizeof(date), "%h %e %H:%M", localtime(&(st.st_mtime)));
    mini_putstr(date);
    // mini_putnbr((long)st.st_mtime);
    mini_putstr("  ");

    // Nom
    mini_putstr(display_name);

    // Gestion Lien Symbolique
    if (S_ISLNK(st.st_mode))
    {
        char buf[1024];
        ssize_t len = readlink(full_path, buf, 1023);
        if (len != -1)
        {
            buf[len] = '\0';
            mini_putstr(" -> ");
            mini_putstr(buf);
        }
    }

    mini_putchar('\n');
}

int main(int argc, char **argv)
{
    MINI_DIR *d;
    struct mini_dirent *dir;
    char *target_dir;

    if (argc >= 2)
        target_dir = argv[1];
    else
        target_dir = ".";

    d = mini_opendir(target_dir);
    if (!d)
    {
        mini_perror("Error: Cannot open directory");
        mini_exit(1);
    }

    while ((dir = mini_readdir(d)) != NULL)
    {
        // pour ignorer les fichiers cachés (commençant par .)
        if (dir->d_name[0] != '.')
        {
            char *full_path = get_full_path(target_dir, dir->d_name);
            if (full_path)
            {
                print_file_info(full_path, dir->d_name);
                mini_free(full_path);
            }
        }
    }

    mini_closedir(d);
    mini_exit(0);
}
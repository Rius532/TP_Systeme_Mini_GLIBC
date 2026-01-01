#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>   // Pour ctime

void ft_putchar(char c) {
    write(1, &c, 1);
}

void ft_putstr(char *str) {
    int i = 0;
    if (!str) return;
    while (str[i])
        i++;
    write(1, str, i);
}

void ft_putnbr(long n) {
    if (n < 0) {
        ft_putchar('-');
        n = -n;
    }
    if (n >= 10)
        ft_putnbr(n / 10);
    ft_putchar((n % 10) + '0');
}
void print_file_type(mode_t mode) {
    if (S_ISDIR(mode)) ft_putchar('d');
    else if (S_ISLNK(mode)) ft_putchar('l');
    else if (S_ISCHR(mode)) ft_putchar('c');
    else if (S_ISBLK(mode)) ft_putchar('b');
    else if (S_ISFIFO(mode)) ft_putchar('p');
    else if (S_ISSOCK(mode)) ft_putchar('s');
    else ft_putchar('-');
}

void print_permissions(mode_t mode) {
    print_file_type(mode);
    ft_putchar((mode & S_IRUSR) ? 'r' : '-');
    ft_putchar((mode & S_IWUSR) ? 'w' : '-');
    ft_putchar((mode & S_IXUSR) ? 'x' : '-');
    ft_putchar((mode & S_IRGRP) ? 'r' : '-');
    ft_putchar((mode & S_IWGRP) ? 'w' : '-');
    ft_putchar((mode & S_IXGRP) ? 'x' : '-');
    ft_putchar((mode & S_IROTH) ? 'r' : '-');
    ft_putchar((mode & S_IWOTH) ? 'w' : '-');
    ft_putchar((mode & S_IXOTH) ? 'x' : '-');
}

void print_file_info(char *filename) {
    struct stat st;

    if (lstat(filename, &st) == -1) {
        ft_putstr("Error stat\n");
        return;
    }

    // Permissions
    print_permissions(st.st_mode);
    ft_putstr("  ");

    // Liens 
    ft_putnbr((long)st.st_nlink);
    ft_putstr("  ");

    // User ID & Group ID
    ft_putnbr((long)st.st_uid);
    ft_putstr("  ");
    ft_putnbr((long)st.st_gid);
    ft_putstr("  ");

    // Taille en octets
    ft_putnbr((long)st.st_size);
    ft_putstr("  ");

    // Date 
    // Si ctime interdit : st.st_mtime
    char *date = ctime(&st.st_mtime);
    //couper le \n à la fin de ctime
    int i = 0;
    while (date[i] && date[i] != '\n') {
        ft_putchar(date[i]);
        i++;
    }
    ft_putstr("  ");

    // Nom du fichier
    ft_putstr(filename);

    // Gestion Lien Symbolique
    if (S_ISLNK(st.st_mode)) {
        char buf[1024];
        ssize_t len = readlink(filename, buf, 1023);
        if (len != -1) {
            buf[len] = '\0';
            ft_putstr(" -> ");
            ft_putstr(buf);
        }
    }

    ft_putchar('\n');
}

int main(void) {
    DIR *d;
    struct dirent *dir;

    d = opendir("."); // On ouvre le dossier courant
    if (!d) {
        ft_putstr("Error: Cannot open directory\n");
        return (1);
    }

    while ((dir = readdir(d)) != NULL) {
        print_file_info(dir->d_name);
        //if (dir->d_name[0] != '.') {
        //    print_file_info(dir->d_name);
        // }
    }
    closedir(d);
    return (0);
}
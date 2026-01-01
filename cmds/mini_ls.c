#include "mini_lib.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>   // Pour ctime


char *get_full_path(char *dir, char *filename) {
    int len_dir = mini_strlen(dir);
    int len_file = mini_strlen(filename);
    
    // On alloue : taille dossier + 1 ('/') + taille fichier + 1 ('\0')
    // mini_calloc initialise tout à 0, donc le '\0' final est garanti.
    char *new_path = (char *)mini_calloc(sizeof(char), len_dir + len_file + 2);
    
    if (!new_path) {
        mini_perror("Erreur allocation path"); // Utilise ta lib pour l'erreur
        return NULL;
    }

    int i = 0;
    
    // 1. Copie du dossier
    // On n'utilise pas mini_strcpy pour éviter l'ambiguïté s/d et gérer le curseur 'i'
    int j = 0;
    while (dir[j]) {
        new_path[i] = dir[j];
        i++;
        j++;
    }

    // 2. Ajout du slash (si pas déjà présent et si le dossier n'est pas vide)
    // Astuce : si dir est juste "/", on évite de faire "//" (même si Unix le tolère)
    if (i > 0 && new_path[i - 1] != '/') {
        new_path[i] = '/';
        i++;
    }

    // 3. Copie du nom de fichier
    j = 0;
    while (filename[j]) {
        new_path[i] = filename[j];
        i++;
        j++;
    }
    
    // Pas besoin d'ajouter '\0' manuellement car mini_calloc l'a fait.
    
    return new_path;
}

void print_file_type(mode_t mode) {
    if (S_ISDIR(mode)) mini_putchar('d');
    else if (S_ISLNK(mode)) mini_putchar('l');
    else if (S_ISCHR(mode)) mini_putchar('c');
    else if (S_ISBLK(mode)) mini_putchar('b');
    else if (S_ISFIFO(mode)) mini_putchar('p');
    else if (S_ISSOCK(mode)) mini_putchar('s');
    else mini_putchar('-');
}

void print_permissions(mode_t mode) {
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

void print_file_info(char *full_path, char *display_name) {
    struct stat st;

    if (lstat(full_path, &st) == -1) {
        mini_putstr("Error stat: ");
        mini_putstr(display_name);
        mini_putchar('\n');
        return;
    }

    // Permissions
    print_permissions(st.st_mode);
    mini_putstr("  ");

    // Liens 
    mini_putnbr((long)st.st_nlink);
    mini_putstr("  ");

    // User ID & Group ID
    mini_putnbr((long)st.st_uid);
    mini_putstr("  ");
    mini_putnbr((long)st.st_gid);
    mini_putstr("  ");

    // Taille en octets
    mini_putnbr((long)st.st_size);
    mini_putstr("  ");

    // Date 
    // Si ctime interdit : st.st_mtime
    char *date = ctime(&st.st_mtime);
    //couper le \n à la fin de ctime
    int i = 0;
    while (date[i] && date[i] != '\n') {
        mini_putchar(date[i]);
        i++;
    }
    mini_putstr("  ");

    // Nom du fichier
    mini_putstr(display_name);

    // Gestion Lien Symbolique
    if (S_ISLNK(st.st_mode)) {
        char buf[1024];
        ssize_t len = readlink(full_path, buf, 1023);        
        if (len != -1) {
            buf[len] = '\0';
            mini_putstr(" -> ");
            mini_putstr(buf);
        }
    }

    mini_putchar('\n');
}

// Change la signature pour accepter les arguments
int main(int argc, char **argv) {
    DIR *d;
    struct dirent *dir;
    char *target_dir;
    if (argc >= 2)
        target_dir = argv[1];
    else
        target_dir = ".";

    d = opendir(target_dir); // On ouvre le dossier cible
    if (!d) {
        mini_putstr("Error: Cannot open directory\n");
        return (1);
    }

    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] != '.') {
            char *full_path = get_full_path(target_dir, dir->d_name);            
            if (full_path) {
                print_file_info(full_path, dir->d_name);
                mini_free(full_path);
            }
        }
    }
    closedir(d);
    return (0);
}
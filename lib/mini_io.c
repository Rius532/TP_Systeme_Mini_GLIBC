#include <unistd.h>
#include <fcntl.h> /* Indispensable pour O_RDONLY, O_CREAT... */
#include <sys/types.h>
#include <sys/stat.h> /* Pour les permissions (0644) */
#include "mini_lib.h"

int IOBUFFER_SIZE = 2048;

MYFILE *mini_fopen(char *file, char mode)
{
    int fd = -1;
    int flags = 0;

    // Permissions par défaut : rw-r--r--
    mode_t permissions = 0644;

    // gestion des modes en flags système
    if (mode == 'r')
        flags = O_RDONLY;
    else if (mode == 'w')
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    else if (mode == 'b')
        flags = O_RDWR | O_CREAT;
    else if (mode == 'a')
        flags = O_WRONLY | O_CREAT | O_APPEND;
    else
        return NULL;

    // Appel system open
    if (flags & O_CREAT)
    {
        fd = open(file, flags, permissions);
    }
    else
    {
        fd = open(file, flags);
    }
    if (fd == -1)
        return NULL;

    // Allocation de la strucutre MYFILE
    MYFILE *stream = (MYFILE *)mini_calloc(sizeof(MYFILE), 1);
    if (stream == NULL)
    {
        close(fd); /* Ne pas laisser un descripteur ouvert si l'alloc échoue */
        return NULL;
    }

    // Initialisation de la structure
    stream->fd = fd;
    stream->ind_read = -1;
    stream->ind_write = -1;
    stream->buffer_read = NULL;  // mini_calloc(sizeof(char), IOBUFFER_SIZE);
    stream->buffer_write = NULL; // mini_calloc(sizeof(char), IOBUFFER_SIZE);

    return stream;
}

int mini_fread(void *buffer, int size_element, int number_element, MYFILE *file)
{
    if (file == NULL || buffer == NULL)
        return -1;

    int total_bytes_needed = size_element * number_element;
    int bytes_copied = 0;
    char *user_buffer = (char *)buffer;

    // Allocation du buffer au 1er appel
    if (file->buffer_read == NULL)
    {
        file->buffer_read = mini_calloc(1, IOBUFFER_SIZE);
        if (file->buffer_read == NULL)
            return -1;
        file->ind_read = -1;
    }

    while (bytes_copied < total_bytes_needed)
    {
        if (file->ind_read == -1 || file->ind_read == IOBUFFER_SIZE)
        {

            // On lit (tjs) IOBUFFER_SIZE éléments
            int n = read(file->fd, file->buffer_read, IOBUFFER_SIZE);

            if (n == -1)
                return -1;
            if (n == 0)
                break; // Fin de fichier réelle

            file->ind_read = 0; // Reset l'index au début
        }

        // Copie des données du buffer interne
        char *internal_buf = (char *)file->buffer_read;
        user_buffer[bytes_copied] = internal_buf[file->ind_read];

        bytes_copied++;
        file->ind_read++;
    }

    return bytes_copied; /* Retourne le nombre d'octets lus (ou à adapter selon le return voulu) [cite: 91] */
}

// Force l'écriture du buffer sur le disque
int mini_fflush(MYFILE *file)
{
    if (file == NULL)
        return -1;
    if (file->buffer_write != NULL && file->ind_write > 0)
    {
        int n = write(file->fd, file->buffer_write, file->ind_write);
        if (n == -1)
            return -1;
        file->ind_write = 0; // on reset l'index du buffer (considère vide)
        return n;
    }
    return 0;
}

int mini_fwrite(void *buffer, int size_element, int number_element, MYFILE *file)
{
    if (file == NULL || buffer == NULL)
        return -1;

    int total_bytes = size_element * number_element;
    char *data = (char *)buffer;
    int written = 0;
    // Allocation du buffer d'écriture
    if (file->buffer_write == NULL)
    {
        file->buffer_write = mini_calloc(1, IOBUFFER_SIZE);
        if (file->buffer_write == NULL)
            return -1;
        file->ind_write = 0;
    }

    while (written < total_bytes)
    {
        // On remplit le buffer interne octet par octet
        char *internal_buf = (char *)file->buffer_write;
        internal_buf[file->ind_write] = data[written];

        file->ind_write++;
        written++;

        // Si le buffer est plein on l'écrit sur le disque
        if (file->ind_write == IOBUFFER_SIZE)
        {
            if (mini_fflush(file) == -1)
            {
                return -1;
            }
        }
    }

    return written;
}

int mini_fclose(MYFILE *file)
{
    if (file == NULL)
        return -1;
    if (mini_fflush(file) == -1)
    {
        // Erreur lors de l'écriture // TODO : Gestion de l'erreur ? on doit continuer dans tt les cas pr vider la memoire
    }

    if (close(file->fd) == -1)
    {
        return -1;
    }

    if (file->buffer_read != NULL)
    {
        mini_free(file->buffer_read);
    }
    if (file->buffer_write != NULL)
    {
        mini_free(file->buffer_write);
    }

    mini_free(file);
    return 0;
}

int mini_fgetc(MYFILE *file)
{
    char c;
    int res = mini_fread(&c, 1, 1, file);
    if (res <= 0)
        return -1;
    return (unsigned char)c;
}

int mini_fputc(MYFILE *file, char c)
{
    int res = mini_fwrite(&c, 1, 1, file);
    if (res <= 0)
        return -1;
    return (unsigned char)c;
}

int mini_putnbr_octal(char *str)
{
    int res = 0;
    int i = 0;

    while (str[i])
    {
        if (str[i] < '0' || str[i] > '7')
        {
            mini_printf("Error: Invalid octal number\n");
            return (-1);
        }
        //  base 8 : on décale de 3 bits ou on multiplie par 8
        res = res * 8 + (str[i] - '0');
        i++;
    }
    return res;
}

char *get_full_path(char *dir, char *filename)
{
    int len_dir = mini_strlen(dir);
    int len_file = mini_strlen(filename);

    // On alloue : taille dossier + 1 ('/') + taille fichier + 1 ('\0')
    // mini_calloc initialise tout à 0, donc le '\0' final est garanti.
    char *new_path = (char *)mini_calloc(sizeof(char), len_dir + len_file + 2);

    if (!new_path)
    {
        mini_perror("Erreur allocation path"); // Utilise ta lib pour l'erreur
        return NULL;
    }

    int i = 0;

    // 1. Copie du dossier
    // On n'utilise pas mini_strcpy pour éviter l'ambiguïté s/d et gérer le curseur 'i'
    int j = 0;
    while (dir[j])
    {
        new_path[i] = dir[j];
        i++;
        j++;
    }

    // 2. Ajout du slash (si pas déjà présent et si le dossier n'est pas vide)
    // Astuce : si dir est juste "/", on évite de faire "//" (même si Unix le tolère)
    if (i > 0 && new_path[i - 1] != '/')
    {
        new_path[i] = '/';
        i++;
    }

    // 3. Copie du nom de fichier
    j = 0;
    while (filename[j])
    {
        new_path[i] = filename[j];
        i++;
        j++;
    }

    // Pas besoin d'ajouter '\0' manuellement car mini_calloc l'a fait.

    return new_path;
}

int mini_strncmp(char *s1, char *s2, int n)
{
    int i = 0;
    while (i < n && s1[i] && s2[i])
    {
        if (s1[i] != s2[i])
            return s1[i] - s2[i];
        i++;
    }
    if (i == n)
        return 0;
    return s1[i] - s2[i];
}

// Pour trouver une variable dans l'env. Retourne l'index ou -1.
int find_env_var(char **env, char *var)
{
    int i = 0;
    int len = mini_strlen(var);
    while (env[i])
    {
        // On cherche "VAR=" au début de la chaine
        if (mini_strncmp(env[i], var, len) == 0 && env[i][len] == '=')
        {
            return i;
        }
        i++;
    }
    return -1;
}
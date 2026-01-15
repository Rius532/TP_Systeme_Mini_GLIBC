#include "mini_lib.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>

#define AT_FDCWD -100

// Structure du noyau Linux
struct linux_dirent64
{
    unsigned long d_ino;
    long d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[];
};

MINI_DIR *mini_opendir(char *filename)
{
    int fd;

    if ((fd = openat(AT_FDCWD, filename, O_RDONLY | __O_DIRECTORY)) == -1)
    {
        return NULL;
    }

    MINI_DIR *dir = mini_calloc(sizeof(MINI_DIR), 1);
    if (dir == NULL)
    {
        close(fd);
        return NULL;
    }

    dir->fd = fd;
    dir->buffer_pos = 0;
    dir->buffer_end = 0;

    return dir;
}

struct mini_dirent *mini_readdir(MINI_DIR *dir)
{
    static struct mini_dirent result;

    if (dir->buffer_pos >= dir->buffer_end)
    {
        int nread = syscall(SYS_getdents64, dir->fd, dir->buffer, 1024);

        if (nread == -1 || nread == 0)
        {
            return NULL;
        }

        dir->buffer_end = nread;
        dir->buffer_pos = 0;
    }

    struct linux_dirent64 *d = (struct linux_dirent64 *)(dir->buffer + dir->buffer_pos);

    result.d_type = d->d_type;

    int i = 0;
    while (d->d_name[i] && i < 255)
    {
        result.d_name[i] = d->d_name[i];
        i++;
    }
    result.d_name[i] = '\0';

    dir->buffer_pos += d->d_reclen;

    return &result;
}

int mini_closedir(MINI_DIR *dir)
{
    if (dir == NULL)
        return -1;

    close(dir->fd);
    mini_free(dir);
    return 0;
}
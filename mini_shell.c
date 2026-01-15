#include "mini_lib.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

int mini_readline(char *buffer, int size)
{
    int ret;
    char c;
    int i = 0;
    while (i < size - 1)
    {
        ret = read(STDIN, &c, 1);
        if (ret <= 0)
            return 0;
        if (c == '\n')
            break;
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    return 1;
}

void parsing_manuel(char *cmd, char **args)
{
    int i = 0;
    int arg_idx = 0;
    int in_word = 0;
    while (cmd[i])
    {
        if (cmd[i] == ' ')
        {
            cmd[i] = '\0';
            in_word = 0;
        }
        else if (in_word == 0)
        {
            args[arg_idx] = &cmd[i];
            arg_idx++;
            in_word = 1;
        }
        i++;
    }
    args[arg_idx] = NULL;
}

int detect_background(char **args)
{
    int i = 0;
    while (args[i] != NULL)
        i++;

    if (i > 0 && mini_strcmp(args[i - 1], "&") == 0)
    {
        args[i - 1] = NULL;
        return 1;
    }
    return 0;
}

char *detect_redirection(char **args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        if (mini_strcmp(args[i], ">") == 0)
        {
            if (args[i + 1] == NULL)
            {
                mini_printf("Syntax error: missing file after >\n");
                return NULL;
            }
            char *filename = args[i + 1];
            args[i] = NULL;
            return filename;
        }
        i++;
    }
    return NULL;
}

void execute_command(char **args, char *redifile, int bg, char **envp)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        mini_perror("Fork failed");
        return;
    }

    if (pid == 0) // FILS
    {
        if (redifile)
        {
            int fd = open(redifile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                mini_perror("Open redirect failed");
                mini_exit(1);
            }
            dup2(fd, STDOUT);
            close(fd);
        }
        execve(args[0], args, envp);
        mini_perror("Execve failed");
        mini_exit(1);
    }
    else // PÈRE
    {
        if (bg)
        {
            mini_printf("[BG] PID: ");
            mini_putnbr((long)pid);
            mini_printf("\n");
        }
        else
        {
            wait(NULL);
        }
    }
}

int main(int argc, char *argv[], char *envp[])
{
    char cmd[512];
    char *args[256];

    while (1)
    {
        // Nettoyage zombies
        int status;
        pid_t zombie;
        while ((zombie = waitpid(-1, &status, WNOHANG)) > 0)
        {
            mini_printf("[BG] Process ");
            mini_putnbr((long)zombie);
            mini_printf(" finished.\n");
        }

        mini_printf("mini-shell$ ");
        mini_flush();

        if (mini_readline(cmd, 512) == 0)
            break;
        if (mini_strlen(cmd) == 0)
            continue;
        if (mini_strcmp(cmd, "exit") == 0)
            break;

        parsing_manuel(cmd, args);
        if (args[0] == NULL)
            continue;

        int bg = detect_background(args);
        char *redi = detect_redirection(args);

        execute_command(args, redi, bg, envp);
    }
    mini_printf("\nBye!\n");
    return 0;
}
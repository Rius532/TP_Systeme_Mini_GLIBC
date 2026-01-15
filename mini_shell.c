#include "mini_lib.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

char **g_env;
char g_base_path[1024];

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

char *resolve_path(char *cmd)
{
    int i = 0;
    while (cmd[i])
    {
        if (cmd[i] == '/')
            return cmd;
        i++;
    }
    static char buffer[2048];

    // On copie le chemin de base
    int b = 0;
    while (g_base_path[b])
    {
        buffer[b] = g_base_path[b];
        b++;
    }
    buffer[b++] = '/';

    // Ajout de la commande
    int c = 0;
    while (cmd[c])
    {
        buffer[b++] = cmd[c++];
    }
    buffer[b] = '\0';

    if (access(buffer, X_OK) == 0)
    {
        return buffer;
    }

    return cmd;
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
        char *final_cmd = resolve_path(args[0]);

        execve(final_cmd, args, envp);
        // execve(args[0], args, envp);
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

int builtin_cd(char **args)
{
    if (!args[1])
        return 1;

    if (chdir(args[1]) != 0)
    {
        mini_perror("cd failed");
        return 1;
    }
    return 1;
}

int builtin_env()
{
    int i = 0;
    while (g_env[i])
    {
        mini_printf(g_env[i]);
        mini_printf("\n");
        i++;
    }
    return 1;
}

int builtin_getenv(char **args)
{
    if (!args[1])
    {
        mini_printf("Usage: mini_getenv <VAR>\n");
        return 1;
    }

    int index = find_env_var(g_env, args[1]);
    if (index != -1)
    {
        char *ptr = g_env[index];
        while (*ptr && *ptr != '=')
            ptr++;
        if (*ptr == '=')
            ptr++;
        mini_printf(ptr);
        mini_printf("\n");
    }
    return 1;
}
int builtin_export(char **args)
{
    if (!args[1])
        return 1;

    char *arg = args[1];
    int i = 0;
    while (arg[i] && arg[i] != '=')
        i++;
    if (arg[i] != '=')
        return 1;

    char key[128];
    int k = 0;
    while (k < i && k < 127)
    {
        key[k] = arg[k];
        k++;
    }
    key[k] = '\0';
    int index = find_env_var(g_env, key);
    if (index != -1)
    {
        g_env[index] = arg;
    }
    else
    {
        int count = 0;
        while (g_env[count])
            count++;
        // Allouer un nouveau tableau plus grand (+2 : un pour la var, un pour NULL)
        // char **new_env = mini_calloc(sizeof(char *), count + 2);
        g_env[count] = arg;
        g_env[count + 1] = NULL;
    }
    return 1;
}

void init_env(char **envp)
{
    int count = 0;
    while (envp[count])
        count++;
    g_env = (char **)mini_calloc(sizeof(char *), count + 100);

    int i = 0;
    while (envp[i])
    {
        // strdup
        g_env[i] = envp[i];
        i++;
    }
    g_env[i] = NULL;
}

int main(int argc, char *argv[], char *envp[])
{
    if (getcwd(g_base_path, 1024) == NULL)
    {
        mini_perror("Init error");
        return 1;
    }
    init_env(envp);
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

        // Interception des commandes internes
        if (mini_strcmp(args[0], "mini_cd") == 0)
        {
            builtin_cd(args);
            continue;
        }
        if (mini_strcmp(args[0], "mini_env") == 0)
        {
            builtin_env();
            continue;
        }
        if (mini_strcmp(args[0], "mini_getenv") == 0)
        {
            builtin_getenv(args);
            continue;
        }
        if (mini_strcmp(args[0], "mini_export") == 0)
        {
            builtin_export(args);
            continue;
        }

        int bg = detect_background(args);
        char *redi = detect_redirection(args);

        execute_command(args, redi, bg, envp);
    }
    mini_printf("\nBye!\n");
    return 0;
}
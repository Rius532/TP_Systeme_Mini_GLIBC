#include "mini_lib.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUFFER_SIZE 2048

typedef struct
{
    char **args;
    char *file_out;
    int mode_out;
    char *file_err;
    int mode_err;
    int background;
} t_cmd;

char g_base_path[1024];
char **g_env;

int mini_readline(int fd, char *buffer, int size)
{
    char c;
    int i = 0;
    int ret;
    while (i < size - 1)
    {
        ret = read(fd, &c, 1);
        if (ret <= 0)
        {
            if (i > 0)
                break;
            return 0;
        }
        if (c == '\n')
            break;
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    return 1;
}

void builtin_cd(char **args)
{
    char *path = args[1];
    if (!path)
    {
        int idx = find_env_var(g_env, "HOME");
        if (idx != -1)
            path = g_env[idx] + 5;
    }
    if (path && chdir(path) != 0)
    {
        mini_perror("cd failed");
    }
}

void builtin_env()
{
    int i = 0;
    while (g_env[i])
    {
        mini_printf(g_env[i]);
        mini_printf("\n");
        i++;
    }
}

void builtin_getenv(char **args)
{
    if (!args[1])
        return;
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
}

void builtin_export(char **args)
{
    if (!args[1])
        return;
    char *arg = args[1];

    int i = 0;
    while (arg[i] && arg[i] != '=')
        i++;
    if (arg[i] != '=')
        return;

    char save = arg[i];
    arg[i] = '\0';
    int index = find_env_var(g_env, arg);
    arg[i] = save;

    if (index != -1)
    {
        g_env[index] = arg;
    }
    else
    {
        int count = 0;
        while (g_env[count])
            count++;
        g_env[count] = arg;
        g_env[count + 1] = NULL;
    }
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
    int b = 0, k = 0;
    while (g_base_path[k])
        buffer[b++] = g_base_path[k++];
    buffer[b++] = '/';
    k = 0;
    while (cmd[k])
        buffer[b++] = cmd[k++];
    buffer[b] = '\0';

    if (access(buffer, X_OK) == 0)
        return buffer;
    return cmd;
}

void parse_command(char **raw_args, t_cmd *cmd)
{
    cmd->args = raw_args;
    cmd->file_out = NULL;
    cmd->mode_out = 0;
    cmd->file_err = NULL;
    cmd->mode_err = 0;
    cmd->background = 0;

    int i = 0, j = 0;
    while (raw_args[i])
    {
        char *t = raw_args[i];
        if (mini_strcmp(t, "&") == 0)
        {
            cmd->background = 1;
            i++;
        }
        else if (mini_strcmp(t, ">") == 0 && raw_args[i + 1])
        {
            cmd->file_out = raw_args[i + 1];
            cmd->mode_out = O_WRONLY | O_CREAT | O_TRUNC;
            i += 2;
        }
        else if (mini_strcmp(t, ">>") == 0 && raw_args[i + 1])
        {
            cmd->file_out = raw_args[i + 1];
            cmd->mode_out = O_WRONLY | O_CREAT | O_APPEND;
            i += 2;
        }
        else if (mini_strcmp(t, "2>") == 0 && raw_args[i + 1])
        {
            cmd->file_err = raw_args[i + 1];
            cmd->mode_err = O_WRONLY | O_CREAT | O_TRUNC;
            i += 2;
        }
        else if (mini_strcmp(t, "2>>") == 0 && raw_args[i + 1])
        {
            cmd->file_err = raw_args[i + 1];
            cmd->mode_err = O_WRONLY | O_CREAT | O_APPEND;
            i += 2;
        }
        else
        {
            cmd->args[j++] = raw_args[i++];
        }
    }
    cmd->args[j] = NULL;
}

void execute_cmd_struct(t_cmd *cmd)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        mini_perror("Fork failed");
        return;
    }

    if (pid == 0) // FILS
    {
        if (cmd->file_out)
        {
            int fd = open(cmd->file_out, cmd->mode_out, 0644);
            if (fd < 0)
            {
                mini_perror("Open stdout failed");
                mini_exit(1);
            }
            dup2(fd, 1);
            close(fd);
        }
        if (cmd->file_err)
        {
            int fd = open(cmd->file_err, cmd->mode_err, 0644);
            if (fd < 0)
            {
                mini_perror("Open stderr failed");
                mini_exit(1);
            }
            dup2(fd, 2);
            close(fd);
        }

        char *final_cmd = resolve_path(cmd->args[0]);
        execve(final_cmd, cmd->args, g_env);
        mini_perror("Execve failed");
        mini_exit(127);
    }
    else // PÈRE
    {
        if (cmd->background)
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

void process_line(char *line)
{
    char *args[256];
    int i = 0, arg_idx = 0, in_word = 0;
    while (line[i])
    {
        if (line[i] == ' ')
        {
            line[i] = '\0';
            in_word = 0;
        }
        else if (in_word == 0)
        {
            args[arg_idx++] = &line[i];
            in_word = 1;
        }
        i++;
    }
    args[arg_idx] = NULL;

    if (args[0] == NULL)
        return;

    if (mini_strcmp(args[0], "exit") == 0)
        mini_exit(0);
    if (mini_strcmp(args[0], "mini_cd") == 0)
    {
        builtin_cd(args);
        return;
    }
    if (mini_strcmp(args[0], "mini_env") == 0)
    {
        builtin_env();
        return;
    }
    if (mini_strcmp(args[0], "mini_getenv") == 0)
    {
        builtin_getenv(args);
        return;
    }
    if (mini_strcmp(args[0], "mini_export") == 0)
    {
        builtin_export(args);
        return;
    }

    t_cmd cmd_struct;
    parse_command(args, &cmd_struct);
    execute_cmd_struct(&cmd_struct);
}
void load_bashrc()
{
    char *home = NULL;
    int i = 0;
    while (g_env[i])
    {
        if (mini_strncmp(g_env[i], "HOME=", 5) == 0)
        {
            home = g_env[i] + 5;
            break;
        }
        i++;
    }
    if (!home)
        return;

    char path[1024];
    int p = 0, k = 0;
    while (home[k])
        path[p++] = home[k++];
    char *rc = "/.mini_bashrc";
    k = 0;
    while (rc[k])
        path[p++] = rc[k++];
    path[p] = '\0';

    int fd = open(path, O_RDONLY);
    if (fd == -1)
        return;

    char buffer[BUFFER_SIZE];
    while (mini_readline(fd, buffer, BUFFER_SIZE))
    {
        if (mini_strlen(buffer) > 0)
            process_line(buffer);
    }
    close(fd);
}

void init_env(char **envp)
{
    int count = 0;
    while (envp[count])
        count++;
    g_env = mini_calloc(sizeof(char *), count + 128);
    int i = 0;
    while (envp[i])
    {
        g_env[i] = envp[i];
        i++;
    }
    g_env[i] = NULL;
}

int main(int argc, char *argv[], char *envp[])
{
    init_env(envp);
    getcwd(g_base_path, 1024);

    // load_bashrc();

    char cmd[BUFFER_SIZE];
    while (1)
    {
        int status;
        while (waitpid(-1, &status, WNOHANG) > 0)
            ;

        mini_printf("mini-shell$ ");
        mini_flush();
        if (mini_readline(0, cmd, BUFFER_SIZE) == 0)
            break;
        process_line(cmd);
    }
    mini_printf("\nBye!\n");
    return 0;
}
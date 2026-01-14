#include "mini_lib.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define BUFFER 512

int mini_readline(char *buffer, int size)
{
    int ret;
    char c;
    int i = 0;
    while (i < size - 1)
    {
        ret = read(STDIN_FILENO, &c, 1);
        if (ret <= 0)
            return 0; // EOF ou erreur
        if (c == '\n')
            break;
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    return 1;
}

// cmd la ligne brute et args le tableau à remplir
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
    // On cherche la fin du tableau
    while (args[i] != NULL)
        i++;

    // Vérification sur le dernier élément (i-1 car args[i] est NULL)
    if (i > 0 && mini_strcmp(args[i - 1], "&") == 0)
    {
        args[i - 1] = NULL; // retire '&' pour que execve ne le lise pas
        return 1;
    }
    return 0;
}

char *detect_redirection(char **args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        // Si on trouve le symbole >
        if (mini_strcmp(args[i], ">") == 0)
        {
            // On vérifie qu'il y a bien un fichier après
            if (args[i + 1] == NULL)
            {
                mini_printf("Erreur de syntaxe : fichier manquant après >\n");
                return NULL;
            }

            char *filename = args[i + 1]; // On capture le nom du fichier
            args[i] = NULL;               // On coupe la commande avant le >
            return filename;
        }
        i++;
    }
    return NULL; // Pas de redirection trouvée
}

void execute_command(char **args, char *redirection_file, int background, char **envp)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        mini_perror("Erreur fork");
        return;
    }

    if (pid == 0)
    // FILS
    {
        if (redirection_file) // != NULL
        {
            int fd = open(redirection_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                mini_perror("Erreur open");
                mini_exit(1);
            }
            if (dup2(fd, STDOUT_FILENO) < 0)
            {
                mini_perror("Erreur dup2");
                mini_exit(1);
            }
            close(fd);
        }
        execve(args[0], args, envp);
        mini_perror("Erreur execve");
        mini_exit(1);
    }
    else if (background)
    // PERE
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

int main(int argc, char *argv[], char *envp[])
{
    char cmd[BUFFER];
    char *args[BUFFER / 2 + 1];

    while (1)
    {
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
        char *redirection = detect_redirection(args);

        execute_command(args, redirection, bg, envp);
    }
    mini_printf("\nBye!\n");
    return 0;
}
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
int is_path(char *cmd)
{
    int i = 0;
    while (cmd[i])
    {
        if (cmd[i] == '/')
            return 1;
        i++;
    }
    return 0;
}

char *find_executable(char *cmd, char **envp)
{
    if (is_path(cmd))
    {
        if (access(cmd, X_OK) == 0)
            return cmd;
        return NULL;
    }

    char *path_var = NULL;
    int i = 0;

    // REcherche de la ligne "PATH=" dans l'environnement
    while (envp[i])
    {
        // On compare manuellement les 5 premiers caractères
        if (envp[i][0] == 'P' && envp[i][1] == 'A' &&
            envp[i][2] == 'T' && envp[i][3] == 'H' &&
            envp[i][4] == '=')
        {
            path_var = envp[i] + 5; // On saute "PATH="
            break;
        }
        i++;
    }

    // Si PATH n'existe pas (env -i), impossible de trouver la commande
    if (!path_var)
        return NULL;

    // Parsing du PATH
    static char buffer[1024]; // pour construire le chemin complet
    int p = 0;

    while (path_var[p])
    {
        int b = 0; // Index dans buffer

        // Copie du dossier courant jusqu'à ':' ou la fin
        while (path_var[p] && path_var[p] != ':')
        {
            buffer[b++] = path_var[p++];
        }

        buffer[b++] = '/';

        int c = 0;
        while (cmd[c])
        {
            buffer[b++] = cmd[c++];
        }
        buffer[b] = '\0';

        if (access(buffer, X_OK) == 0) // test d'execution
        {
            return buffer;
        }

        if (path_var[p] == ':')
        {
            p++;
        }
    }

    return NULL; // Commande introuvable
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
        char *valid_path = find_executable(args[0], envp);

        if (valid_path == NULL)
        {
            mini_printf("Command not found: ");
            mini_printf(args[0]);
            mini_printf("\n");
            mini_exit(127);
        }

        execve(valid_path, args, envp);
        // execve(args[0], args, envp);
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
        // Nettoyage des zombies s'il y en a
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
        char *redirection = detect_redirection(args);

        execute_command(args, redirection, bg, envp);
    }
    mini_printf("\nBye!\n");
    return 0;
}
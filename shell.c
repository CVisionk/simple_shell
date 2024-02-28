#include "main.h"

/**
 * tokenize_input - Tokenizes the input command and arguments.
 * @buffer: The input buffer containing the command and arguments.
 * @args: An array to store the tokenized arguments.
 *
 * Return: The number of arguments tokenized.
 */
int tokenize_input(char *buffer, char *args[])
{
    char *token;
    int arg_count = 0;

    token = strtok(buffer, " ");
    while (token != NULL && arg_count < MAX_ARGS - 1)
    {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;

    return arg_count;
}

/**
 * execute_command - Executes a command with arguments using execve.
 * @command: The command to be executed.
 * @args: The arguments for the command.
 *
 * Return: This function does not return if successful.
 *         Otherwise, it exits with an error message.
 */
void execute_command(char *command, char *args[])
{
    if (execve(command, args, environ) == -1)
    {
        perror("execve");
        exit(1);
    }
}

/**
 * search_and_execute - Searches for the executable file in PATH and executes it.
 * @command: The command to be executed.
 * @args: The arguments for the command.
 *
 * This function searches for the executable file in the PATH environment
 * variable and executes it if found. Otherwise, it prints an error message.
 */
void search_and_execute(char *command, char *args[])
{
    char *path = getenv("PATH");
    char *dir;
    char filepath[256];

    if (path == NULL)
    {
        strcpy(filepath, "/bin/");
        strcat(filepath, command);
        execute_command(filepath, args);
    }
    else
    {
        dir = strtok(path, ":");
        while (dir != NULL)
        {
            strcpy(filepath, dir);
            strcat(filepath, "/");
            strcat(filepath, command);
            if (access(filepath, X_OK) == 0)
            {
                execute_command(filepath, args);
            }
            dir = strtok(NULL, ":");
        }
        printf("%s: command not found\n", command);
    }
}

/**
 * handle_child_process - Handles the child process by executing the command.
 * @args: The arguments for the command.
 *
 * This function checks if the command includes an absolute path. If it does,
 * it executes the command. Otherwise, it searches for the command in PATH.
 */
void handle_child_process(char *args[])
{
    if (args[0][0] == '/')
    {
        execute_command(args[0], args);
    }
    else
    {
        search_and_execute(args[0], args);
    }
}

/**
 * fork_and_wait - Forks a child process and waits for it.
 * @args: The arguments for the command.
 *
 * This function forks a child process. If the fork fails, it exits with an
 * error message. If successful, the child process executes the command and
 * the parent process waits for the child to finish.
 */
void fork_and_wait(char *args[])
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        /* Child process */
        handle_child_process(args);
    }
    else
    {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * main - Entry point of the program.
 *
 * This function repeatedly prompts the user for a command and executes it.
 * It terminates when the user enters "exit".
 *
 * Return: Always returns 0.
 */
int main(void)
{
    char *buffer;
    size_t bufsize = BUFFER_SIZE;
    char *args[MAX_ARGS];
    ssize_t getline_status;

    buffer = (char *)malloc(bufsize * sizeof(char));
    if (buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }

    while (1)
    {
        printf("#$ ");
        fflush(stdout);

        getline_status = getline(&buffer, &bufsize, stdin);

        if (getline_status == -1)
        {
            if (feof(stdin))
            {
                printf("\n");
                break;
            }
            else
            {
                perror("getline");
                continue;
            }
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "exit") == 0)
        {
            break;
        }

        tokenize_input(buffer, args);
        fork_and_wait(args);
    }

    free(buffer);
    return 0;
}
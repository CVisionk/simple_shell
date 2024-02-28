#include "main.h"


/**
 * Tokenizes the input command and arguments.
 */
int tokenize_input(char *buffer, char *args[]) {
    char *token;
    int arg_count = 0;

    token = strtok(buffer, " \t\n");
    while (token != NULL && arg_count < MAX_ARGS - 1) {
        args[arg_count++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[arg_count] = NULL;

    return arg_count;
}

/**
 * Executes a command with arguments using execve.
 */
void execute_command(char *command, char *args[]) {
    if (execve(command, args, environ) == -1) {
        fprintf(stderr, "%s: %s\n", command, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

/**
 * Searches for the executable file in PATH and executes it.
 */
void search_and_execute(char *command, char *args[]) {
    if (strchr(command, '/')) {
        execute_command(command, args);
        return;
    }

    char *path = getenv("PATH");
    char *dir;
    char filepath[256];

    if (path != NULL) {
        char *path_copy = strdup(path);
        dir = strtok(path_copy, ":");
        while (dir != NULL) {
            snprintf(filepath, sizeof(filepath), "%s/%s", dir, command);
            if (access(filepath, X_OK) == 0) {
                execute_command(filepath, args);
                free(path_copy);
                return;
            }
            dir = strtok(NULL, ":");
        }
        free(path_copy);
    }

    fprintf(stderr, "%s: command not found\n", command);
}

/**
 * Checks if the command is a built-in command.
 */
int is_builtin_command(char *command) {
    return strcmp(command, "cd") == 0;
}

/**
 * Handles built-in commands.
 */
void handle_builtin_commands(char *args[]) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || chdir(args[1]) != 0) {
            perror("cd");
        }
    }
}

/**
 * Main shell loop.
 */
int main(void) {
    char *buffer;
    size_t bufsize = BUFFER_SIZE;
    char *args[MAX_ARGS];
    ssize_t getline_status;

    buffer = malloc(bufsize * sizeof(char));
    if (buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(EXIT_FAILURE);
    }

    while (1) {
     	if(isatty(STDIN_FILENO))
      {
        printf("$ ");
      }
        fflush(stdout);

        getline_status = getline(&buffer, &bufsize, stdin);
        if (getline_status == -1) {
            if (feof(stdin)) {
                //printf("\n");
                break;
            } else {
                perror("getline");
                continue;
            }
        }

        tokenize_input(buffer, args);
        if (args[0] == NULL) continue; // Skip empty commands

        if (is_builtin_command(args[0])) {
            handle_builtin_commands(args);
        } else {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                // Child process
                search_and_execute(args[0], args);
                exit(EXIT_FAILURE); // Exit if execve fails
            } else {
                // Parent process
                waitpid(pid, NULL, 0);
            }
        }
    }

    free(buffer);
    return 0;
}
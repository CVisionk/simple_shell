#include "main.h"

/* Function to tokenize the input command and arguments */
int tokenize_input(char *buffer, char *args[]) {
    char *token;
    int arg_count = 0;

    token = strtok(buffer, " ");
    while (token != NULL && arg_count < MAX_ARGS - 1) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL; /* Null-terminate the argument list */

    return arg_count;
}

/* Function to execute a command */
void execute_command(char *command, char *args[]) {
    if (execve(command, args, environ) == -1) {
        perror("execve");
        exit(1);
    }
}

/* Function to search for the executable file in PATH */
void search_and_execute(char *command, char *args[]) {
    char *path = getenv("PATH");
    char *dir;
    char filepath[256]; /* Maximum path length */

    if (path == NULL) {
        /* If PATH environment variable is not set, use /bin */
        strcpy(filepath, "/bin/");
        strcat(filepath, command);
        execute_command(filepath, args);
    } else {
        /* Tokenize PATH to get individual directories */
        dir = strtok(path, ":");
        while (dir != NULL) {
            /* Construct full path to executable */
            strcpy(filepath, dir);
            strcat(filepath, "/");
            strcat(filepath, command);
            if (access(filepath, X_OK) == 0) {
                /* If executable file found, execute it */
                execute_command(filepath, args);
            }
            /* Move to the next directory in PATH */
            dir = strtok(NULL, ":");
        }
        /* If executable not found in any directory in PATH */
        printf("%s: command not found\n", command);
        /*exit(1);*/
    }
}

/* Function to handle the child process */
void handle_child_process(char *args[]) {
    /* Check if the command includes an absolute path */
    if (args[0][0] == '/') {
        execute_command(args[0], args);
    } else {
        search_and_execute(args[0], args);
    }
}

/* Function to fork a child process and wait for it */
void fork_and_wait(char *args[]) {
    /* Fork a child process */
    pid_t pid = fork();

    if (pid == -1) {
        /* Error handling for fork failure */
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        /* Child process */
        handle_child_process(args);
    } else {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);
    }
}
int main(void) {
    char *buffer;
    size_t bufsize = BUFFER_SIZE;
    char *args[MAX_ARGS]; /* Maximum of 64 arguments */
    ssize_t getline_status;

    buffer = (char *)malloc(bufsize * sizeof(char));
    if (buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }

    while (1) {
        printf("$ ");
        fflush(stdout); /* Ensure prompt is printed immediately. */

        getline_status = getline(&buffer, &bufsize, stdin);
        
        /* Check for EOF or read error. */
        if (getline_status == -1) {
            if (feof(stdin)) {
                /* End of input (EOF), break the loop. */
                printf("\n"); /* Print a newline for a clean exit. */
                break;
            } else {
                /* Handle getline error. */
                perror("getline");
                continue;
            }
        }

        /* Remove newline character. */
        buffer[strcspn(buffer, "\n")] = '\0';

        /* Check if user entered "exit". */
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        /* Tokenize the input command and arguments. */
        tokenize_input(buffer, args);

        /* Fork a child process and wait for it. */
        fork_and_wait(args);
    }

    free(buffer);
    return 0;
}
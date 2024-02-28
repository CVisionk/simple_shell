#include "main.h"

/**
 * tokenize_input - Tokenizes the input command and arguments.
 *
 * @buffer: The input buffer containing the command and arguments.
 * @args: An array to store the tokenized arguments.
 *
 * Return: The number of arguments tokenized.
 */
int tokenize_input(char *buffer, char *args[])
{
	char *token;
	int arg_count = 0;

	token = strtok(buffer, " \t\n");
	while (token != NULL && arg_count < MAX_ARGS - 1)
	{
		args[arg_count++] = token;
		token = strtok(NULL, " \t\n");
	}
	args[arg_count] = NULL;

	return (arg_count);
}

/**
 * execute_command - Executes a command with arguments using execve.
 *
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
		fprintf(stderr, "%s: %s\n", command, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

/**
 * search_and_execute - Searches for the executable in PATH and executes it.
 *
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

	if (strchr(command, '/'))
	{
		execute_command(command, args);
		return;
	}



	if (path != NULL)
	{
		char *path_copy = strdup(path);
		dir = strtok(path_copy, ":");

		while (dir != NULL)
		{
			snprintf(filepath, sizeof(filepath), "%s/%s", dir, command);
			if (access(filepath, X_OK) == 0)
			{
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
 * is_builtin_command - Checks if the command is a built-in command.
 *
 * @command: The command to check.
 *
 * Return: 1 if the command is built-in, 0 otherwise.
 */
int is_builtin_command(char *command)
{
	return (strcmp(command, "cd") == 0);
}

/**
 * handle_builtin_commands - Handles built-in commands.
 *
 * @args: The arguments for the command.
 */
void handle_builtin_commands(char *args[])
{
	if (strcmp(args[0], "cd") == 0)
	{
		if (args[1] == NULL || chdir(args[1]) != 0)
		{
			perror("cd");
		}
	}
}

/**
 * main - Main shell loop.
 *
 * This function repeatedly prompts the user for a command, executes it,
 * and handles built-in commands. It terminates when the user enters "exit".
 *
 * Return: Always returns 0.
 */
int main(void)
{
	char *buffer;
	size_t bufsize = BUFFER_SIZE;
	char *args[MAX_ARGS];
	ssize_t getline_status;

	buffer = malloc(bufsize * sizeof(char));
	if (buffer == NULL)
	{
		perror("Unable to allocate buffer");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		if (isatty(STDIN_FILENO))
		{
			printf("$ ");
		}
		fflush(stdout);

		getline_status = getline(&buffer, &bufsize, stdin);
		if (getline_status == -1)
		{
			if (feof(stdin))
			{
				break;
			}
			else
			{
				perror("getline");
				continue;
			}
		}

		tokenize_input(buffer, args);
		if (args[0] == NULL)
		{
			continue;
		}

		if (is_builtin_command(args[0]))
		{
			handle_builtin_commands(args);
		}
		else
		{
			pid_t pid = fork();
			
			if (pid == -1)
			{
				perror("fork");
				exit(EXIT_FAILURE);
			}
			else if (pid == 0)
			{
				search_and_execute(args[0], args);
				exit(EXIT_FAILURE);
			}
			else
			{
				waitpid(pid, NULL, 0);
			}
		}
	}

	free(buffer);
	return 0;
}
